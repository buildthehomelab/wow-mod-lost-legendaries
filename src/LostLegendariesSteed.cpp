/*
 * mod-lost-legendaries: Foror's Fabled Steed takes to the air.
 *
 * Chromatic Mount (24576) is a ground mount: a mounted aura and +100% mounted speed. A drake
 * should fly once its rider can, so while the mount is up the rider also gets one of two
 * server-only auras, each a single "mounted flight speed" effect like the one every flying mount
 * spell carries:
 *   90350  +150% flight speed   riding skill 225 (Expert)
 *   90351  +280% flight speed   riding skill 300 (Artisan)
 * The core does the rest from that effect: it switches flying on and off for the client and
 * sets the flight speed. The client doesn't know these two spells, so they show no buff; the
 * mount's own buff is the one players see and cancel.
 *
 * Where it flies is the core's rule, not ours. Both auras carry the same "only in flying areas"
 * attribute as stock flying mounts, so SpellInfo::CheckLocation allows them only where a Swift
 * Blue Gryphon would work (Outland, and Northrend with Cold Weather Flying), and the core removes
 * them by itself when the rider leaves such an area. The steed then carries on as a ground mount,
 * and its slow fall softens the landing. On a vanilla-only realm nothing changes: there is no
 * flying skill to have and nowhere to fly.
 *
 * The auras are passive, so they are not saved at logout. After a login or an area change the
 * area-update hook puts the right one back if the rider is still mounted.
 *
 * Released under the MIT License.
 */

#include "LostLegendaries.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"

#include <array>

namespace
{
    constexpr uint32 SPELL_CHROMATIC_MOUNT = 24576;
    constexpr uint32 SPELL_STEED_FLIGHT = 90350;       // +150% mounted flight speed
    constexpr uint32 SPELL_STEED_FLIGHT_FAST = 90351;  // +280% mounted flight speed

    constexpr uint16 RIDING_SKILL_EXPERT = 225;
    constexpr uint16 RIDING_SKILL_ARTISAN = 300;

    constexpr std::array<uint32, 2> flightSpells = { SPELL_STEED_FLIGHT, SPELL_STEED_FLIGHT_FAST };

    // The flight aura this rider should have right now, or 0 for none. areaId is passed in because
    // during an area update the player's own area id is still the old one.
    uint32 WantedFlightSpell(Player* player, uint32 areaId)
    {
        // not in world: the mount aura is being loaded at login, before the player has a zone
        if (!LostLegendaries::config.enabled || !LostLegendaries::config.steedFlying || !player->IsInWorld()
            || !player->IsAlive() || !player->HasAura(SPELL_CHROMATIC_MOUNT))
            return 0;

        uint16 riding = player->GetSkillValue(SKILL_RIDING);
        if (riding < RIDING_SKILL_EXPERT)
            return 0;

        uint32 spellId = riding >= RIDING_SKILL_ARTISAN ? SPELL_STEED_FLIGHT_FAST : SPELL_STEED_FLIGHT;

        // the same two checks the core makes when a flying mount is cast
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo || spellInfo->CheckLocation(player->GetMapId(), player->GetZoneId(), areaId, player) != SPELL_CAST_OK)
            return 0;

        Battlefield* battlefield = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId());
        if (battlefield && !battlefield->CanFlyIn())
            return 0;

        return spellId;
    }

    void UpdateSteedFlight(Player* player, uint32 areaId)
    {
        uint32 wanted = WantedFlightSpell(player, areaId);

        for (uint32 spellId : flightSpells)
            if (spellId != wanted)
                player->RemoveAurasDueToSpell(spellId);

        if (wanted && !player->HasAura(wanted))
            player->AddAura(wanted, player);
    }
}

// 24576 - Chromatic Mount
class spell_lost_legendaries_chromatic_mount : public AuraScript
{
    PrepareAuraScript(spell_lost_legendaries_chromatic_mount);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* player = GetTarget()->ToPlayer())
            UpdateSteedFlight(player, player->GetAreaId());
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        for (uint32 spellId : flightSpells)
            GetTarget()->RemoveAurasDueToSpell(spellId);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_lost_legendaries_chromatic_mount::HandleApply, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_lost_legendaries_chromatic_mount::HandleRemove, EFFECT_0, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
    }
};

class LostLegendariesSteedPlayerScript : public PlayerScript
{
public:
    LostLegendariesSteedPlayerScript() : PlayerScript("LostLegendariesSteedPlayerScript", { PLAYERHOOK_ON_UPDATE_AREA }) { }

    // Runs before the core drops area-bound auras for the new area. Covers riding into a place
    // where flying is allowed, and the first area update after logging in mounted.
    void OnPlayerUpdateArea(Player* player, uint32 /*oldArea*/, uint32 newArea) override
    {
        if (player->HasAura(SPELL_CHROMATIC_MOUNT))
            UpdateSteedFlight(player, newArea);
    }
};

void AddLostLegendariesSteedScripts()
{
    RegisterSpellScript(spell_lost_legendaries_chromatic_mount);
    new LostLegendariesSteedPlayerScript();
}
