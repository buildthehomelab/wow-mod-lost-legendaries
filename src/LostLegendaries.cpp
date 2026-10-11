/*
 * mod-lost-legendaries
 *
 * Legendary items that exist in the game data but never drop, back in the game.
 *
 * This file handles the drops. The module's world database updates add one loot row per item:
 *   Talisman of Binding Shard (17782)     Baron Geddon
 *   Talisman of Binding Fragment (17783)  Garr
 *   Shard of the Defiler (17142)          Lord Kazzak
 *   Glaive of the Defender (23051)        Sapphiron (mod-individual-progression's Naxxramas 40)
 *   Foror's Fabled Steed (20221)          Nefarian
 * The loot rows only say where the items drop. The chance is set here, on every roll of one of
 * these items, from the item's Chance setting, and with the module switched off the roll always
 * fails. So the config works without touching the database again. The core still applies
 * Rate.Drop.Item.Legendary on top, as it does for the Bindings of the Windseeker (a chance of
 * 100 always drops, whatever the rate).
 *
 * The rows are also checked at every startup and put back if one is missing. Two things
 * make that necessary: the module's SQL can run before the level 60 Sapphiron exists (on a new
 * database it sorts ahead of mod-individual-progression's), and mod-individual-progression
 * deletes and rebuilds its Naxxramas 40 loot whenever its own SQL file changes.
 *
 * Foror's Fabled Steed is a mount. Its spell, Chromatic Mount (24576), was built for creature
 * 15135 "Chromatic Drake Mount", but the 3.3.5a spell data points it at 18768 "Brazen Taxi", the
 * bronze drake from the Caverns of Time. The mount's look comes from that creature on the server
 * side, so pointing the spell back at 15135 here is enough to make it a chromatic drake again; no
 * client patch is involved. LostLegendariesSteed.cpp lets it fly where flying is allowed.
 *
 * The Ashbringer quest chain is in LostLegendariesAshbringer.cpp.
 *
 * Released under the MIT License.
 */

#include "LostLegendaries.h"

#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "LootMgr.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"

#include <algorithm>
#include <array>
#include <string>

LostLegendaries::Config LostLegendaries::config;

namespace
{
    constexpr uint32 SPELL_CHROMATIC_MOUNT = 24576;
    constexpr uint32 NPC_CHROMATIC_DRAKE_MOUNT = 15135;

    struct LostDrop
    {
        uint32 itemId;
        uint32 lootId;        // creature_loot_template entry of the boss
        char const* comment;  // the loot row's Comment, as in the module's SQL
        char const* chanceOption;
        float defaultChance;  // percent; also the Chance written in the loot row
        float chance;         // percent from the config, 0 = never
    };

    std::array<LostDrop, 5> drops =
    {{
        { 17782, 12056,  "Baron Geddon - Talisman of Binding Shard",          "LostLegendaries.TalismanOfBinding.Chance",   3.0f, 3.0f },
        { 17783, 12057,  "Garr - Talisman of Binding Fragment",               "LostLegendaries.TalismanOfBinding.Chance",   3.0f, 3.0f },
        { 17142, 12397,  "Lord Kazzak - Shard of the Defiler",                "LostLegendaries.ShardOfTheDefiler.Chance",   3.0f, 3.0f },
        { 23051, 351018, "Sapphiron (Naxxramas 40) - Glaive of the Defender", "LostLegendaries.GlaiveOfTheDefender.Chance", 3.0f, 3.0f },
        { 20221, 11583,  "Nefarian - Foror's Fabled Steed",                   "LostLegendaries.FororsFabledSteed.Chance",   1.0f, 1.0f },
    }};
}

class LostLegendariesWorldScript : public WorldScript
{
public:
    LostLegendariesWorldScript() : WorldScript("LostLegendariesWorldScript",
        { WORLDHOOK_ON_AFTER_CONFIG_LOAD, WORLDHOOK_ON_LOAD_CUSTOM_DATABASE_TABLE }) { }

    // Runs once at startup, after the config and the database updates and before the core loads
    // creatures and loot, so a row added here drops from the first kill.
    void OnLoadCustomDatabaseTable() override
    {
        if (!LostLegendaries::config.enabled)
            return;

        for (LostDrop const& drop : drops)
        {
            if (WorldDatabase.Query("SELECT 1 FROM `creature_loot_template` WHERE `Entry` = {} AND `Item` = {}", drop.lootId, drop.itemId))
                continue;

            if (!WorldDatabase.Query("SELECT 1 FROM `creature_template` WHERE `lootid` = {} LIMIT 1", drop.lootId))
            {
                LOG_INFO("server.loading", "mod-lost-legendaries: no creature uses loot {}, so item {} has no source ({})",
                    drop.lootId, drop.itemId, drop.comment);
                continue;
            }

            std::string comment = drop.comment;
            WorldDatabase.EscapeString(comment);
            WorldDatabase.DirectExecute("INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) "
                "VALUES ({}, {}, 0, {}, 0, 1, 0, 1, 1, '{}')", drop.lootId, drop.itemId, drop.defaultChance, comment);
            LOG_WARN("server.loading", "mod-lost-legendaries: the loot row for item {} was missing and has been added again ({})",
                drop.itemId, drop.comment);
        }
    }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        LostLegendaries::config.enabled = sConfigMgr->GetOption<bool>("LostLegendaries.Enable", true);
        LostLegendaries::config.announceAshbringer = sConfigMgr->GetOption<bool>("LostLegendaries.Ashbringer.Announce", true);
        LostLegendaries::config.steedFlying = sConfigMgr->GetOption<bool>("LostLegendaries.FororsFabledSteed.Flying", true);
        for (LostDrop& drop : drops)
            drop.chance = std::clamp(sConfigMgr->GetOption<float>(drop.chanceOption, drop.defaultChance), 0.0f, 100.0f);

        LOG_INFO("server.loading", "mod-lost-legendaries: {}, drop chances: Talisman of Binding {}%, Shard of the Defiler {}%, Glaive of the Defender {}%, Foror's Fabled Steed {}%",
            LostLegendaries::config.enabled ? "enabled" : "disabled", drops[0].chance, drops[2].chance, drops[3].chance, drops[4].chance);
    }
};

class LostLegendariesGlobalScript : public GlobalScript
{
public:
    LostLegendariesGlobalScript() : GlobalScript("LostLegendariesGlobalScript",
        { GLOBALHOOK_ON_ITEM_ROLL, GLOBALHOOK_ON_LOAD_SPELL_CUSTOM_ATTR }) { }

    // Runs once per spell while the core loads spell data. Not tied to Enable: the spell is only
    // reachable through the item, and a config reload could not undo it anyway.
    void OnLoadSpellCustomAttr(SpellInfo* spell) override
    {
        if (spell->Id == SPELL_CHROMATIC_MOUNT)
            spell->Effects[EFFECT_0].MiscValue = NPC_CHROMATIC_DRAKE_MOUNT;
    }

    // Runs for every loot row the server rolls, so it has to stay this cheap. Returning false
    // fails the roll.
    bool OnItemRoll(Player const* /*player*/, LootStoreItem const* item, float& chance, Loot& /*loot*/, LootStore const& /*store*/) override
    {
        // a reference row's itemid is the reference id, not an item
        if (item->reference)
            return true;

        for (LostDrop const& drop : drops)
        {
            if (item->itemid != drop.itemId)
                continue;

            if (!LostLegendaries::config.enabled || drop.chance <= 0.0f)
                return false;

            chance = drop.chance;
            return true;
        }

        return true;
    }
};

void AddLostLegendariesScripts()
{
    new LostLegendariesWorldScript();
    new LostLegendariesGlobalScript();
}
