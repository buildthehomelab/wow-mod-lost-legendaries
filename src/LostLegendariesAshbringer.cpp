/*
 * mod-lost-legendaries: the Ashbringer quest chain.
 *
 * Five quests from Tirion Fordring turn the Corrupted Ashbringer into Ashbringer (13262). The
 * quests are plain database rows; this file holds the parts that need code.
 *
 * - Quest 1's objective is "hear High Inquisitor Fairbanks' tale". Fairbanks only talks during
 *   the stock Corrupted Ashbringer event in the Scarlet Monastery's Cathedral, and his last line
 *   is reached by picking the option on gossip menu 7268. That pick gives the credit.
 *
 * - Quest 4 is a boss fight. While a player has it unfinished, Tirion's gossip gets one more
 *   line: "I am ready, Tirion. Begin the cleansing." Picking it makes Tirion start, and five
 *   seconds later the Shade of Highlord Mograine appears in front of him. Nothing is summoned
 *   until someone picks that line, so talking to Tirion for any other reason is safe, and the
 *   same line calls the shade back after a wipe.
 *
 *   The shade goes for the player who called it, if they are still close. It leaves when the
 *   group wipes, when it is led away (the core's own leash: 30 yards from where it appeared unless
 *   it is being damaged, and never more than 80), or when nobody fights it for five minutes.
 *   When it dies, every player nearby who has the quest gets the credit, whoever hit it first.
 *
 *   Tirion is a level 61 quest giver with no fight in him, and the turn-in needs him alive, so he
 *   is flagged immune to NPCs for as long as a shade exists. The shade removes the flag on every
 *   way out (death, evade, idle despawn). The flag is never written to the database.
 *
 * - Finishing the chain is announced to the whole server.
 *
 * With the module switched off no credit is given, the gossip line is not offered, and the
 * chain's quests can't be completed.
 *
 * Released under the MIT License.
 */

#include "LostLegendaries.h"

#include "Creature.h"
#include "CreatureScript.h"
#include "GossipDef.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "QuestDef.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "StringFormat.h"
#include "WorldSessionMgr.h"

namespace
{
    constexpr uint32 NPC_HIGH_INQUISITOR_FAIRBANKS = 4542;
    constexpr uint32 NPC_TIRION_FORDRING = 1855;
    constexpr uint32 NPC_SHADE_OF_MOGRAINE = 9501300;

    constexpr uint32 GOSSIP_MENU_FAIRBANKS_LAST = 7268;  // "But his son is dead."
    constexpr uint32 GOSSIP_ACTION_BEGIN_CLEANSING = 9501303;  // our line on Tirion's menu

    constexpr uint32 QUEST_BLADE_OF_THE_HIGHLORD = 9501300;  // first: hear Fairbanks
    constexpr uint32 QUEST_WHAT_THE_BLADE_REMEMBERS = 9501303;  // defeat the shade
    constexpr uint32 QUEST_THE_ASHBRINGER = 9501304;  // last: rewards Ashbringer

    constexpr float SHADE_SEARCH_RANGE = 100.0f;
    constexpr float SHADE_SUMMON_DISTANCE = 10.0f;  // yards in front of Tirion
    constexpr float SHADE_INTRO_ATTACK_RANGE = 25.0f;  // inside the core's 30 yard leash
    constexpr float SHADE_LEASH_RANGE = 80.0f;
    constexpr float SHADE_CREDIT_RANGE = 100.0f;
    constexpr uint32 SHADE_INTRO_MS = 5 * IN_MILLISECONDS;
    constexpr uint32 SHADE_IDLE_DESPAWN_MS = 5 * MINUTE * IN_MILLISECONDS;

    bool IsAshbringerQuest(uint32 questId)
    {
        return questId >= QUEST_BLADE_OF_THE_HIGHLORD && questId <= QUEST_THE_ASHBRINGER;
    }

    // Tirion offers to start only to a player who still needs the kill, and only while no shade
    // is up.
    bool CanBeginCleansing(Player* player, Creature* tirion)
    {
        return LostLegendaries::config.enabled && tirion->GetEntry() == NPC_TIRION_FORDRING && tirion->IsAlive()
            && player->GetQuestStatus(QUEST_WHAT_THE_BLADE_REMEMBERS) == QUEST_STATUS_INCOMPLETE
            && !tirion->FindNearestCreature(NPC_SHADE_OF_MOGRAINE, SHADE_SEARCH_RANGE);
    }

    // Tirion speaks; the shade follows five seconds later.
    void BeginCleansing(Creature* tirion, Player* player)
    {
        tirion->Say("Stand ready. Whatever the lich left in this steel will not go quietly.", LANG_UNIVERSAL, player);

        // the event belongs to Tirion, so he is still there when it runs; the player may not be
        tirion->m_Events.AddEventAtOffset([tirion, playerGUID = player->GetGUID()]()
        {
            // two players can pick the line inside the same five seconds
            if (!tirion->IsAlive() || tirion->FindNearestCreature(NPC_SHADE_OF_MOGRAINE, SHADE_SEARCH_RANGE))
                return;

            if (Creature* shade = tirion->SummonCreature(NPC_SHADE_OF_MOGRAINE, tirion->GetNearPosition(SHADE_SUMMON_DISTANCE, 0.0f), TEMPSUMMON_MANUAL_DESPAWN))
                shade->AI()->SetGUID(playerGUID);
        }, 5s);
    }
}

enum ShadeTexts
{
    SAY_SHADE_SUMMONED = 0,
    SAY_SHADE_MEMORY   = 1,
    SAY_SHADE_FRENZY   = 2,
    SAY_SHADE_DEATH    = 3
};

enum ShadeSpells
{
    SPELL_CLEAVE             = 15284,  // 110% melee damage to the target and its nearest allies
    SPELL_MORTAL_STRIKE      = 16856,  // 200% weapon damage, healing taken halved for 5 s
    SPELL_SHADOW_BOLT_VOLLEY = 27646,  // about 700 Shadow damage to every enemy within 30 yards
    SPELL_FRENZY             = 28131   // +25% damage, +40% attack speed, lasts 5 minutes
};

enum ShadeEvents
{
    EVENT_CLEAVE = 1,
    EVENT_MORTAL_STRIKE,
    EVENT_SHADOW_BOLT_VOLLEY,
    EVENT_MEMORY
};

struct boss_lost_legendaries_mograine_shade : public ScriptedAI
{
    boss_lost_legendaries_mograine_shade(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        if (summoner && summoner->GetEntry() == NPC_TIRION_FORDRING)
            _tirionGUID = summoner->GetGUID();
        ProtectTirion(true);

        // a few seconds to read the name before it swings
        me->SetImmuneToPC(true);
        _introTimer = SHADE_INTRO_MS;
        Talk(SAY_SHADE_SUMMONED);
    }

    // The player who asked Tirion to begin.
    void SetGUID(ObjectGuid const& guid, int32 /*id*/) override
    {
        _summonerGUID = guid;
    }

    void Reset() override
    {
        _events.Reset();
        _saidFrenzy = false;
        _idleTimer = SHADE_IDLE_DESPAWN_MS;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_CLEAVE, 6s, 9s);
        _events.ScheduleEvent(EVENT_MORTAL_STRIKE, 10s, 14s);
        _events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, 12s, 16s);
        _events.ScheduleEvent(EVENT_MEMORY, 20s, 30s);
    }

    // No reset to full health and no walk home: the shade leaves, and Tirion calls it back.
    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        ProtectTirion(false);
        me->DespawnOrUnsummon();
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_SHADE_DEATH);
        ProtectTirion(false);
        if (Creature* tirion = GetTirion())
            tirion->Say("He is free. And at last, the blade is silent.", LANG_UNIVERSAL);

        // It dies in the open world, where anyone can land the first hit and own the kill. Give
        // the credit to everyone nearby who needs it instead (a second credit changes nothing).
        me->GetMap()->DoForAllPlayers([this](Player* player)
        {
            if (player->IsWithinDist(me, SHADE_CREDIT_RANGE) && player->GetQuestStatus(QUEST_WHAT_THE_BLADE_REMEMBERS) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(NPC_SHADE_OF_MOGRAINE);
        });

        me->DespawnOrUnsummon(1min);
    }

    void UpdateAI(uint32 diff) override
    {
        if (_introTimer)
        {
            if (_introTimer > diff)
            {
                _introTimer -= diff;
                return;
            }

            _introTimer = 0;
            me->SetImmuneToPC(false);

            // Go for the player who called it, if they stayed. Anyone further away than this would
            // drag it straight past the core's leash; it can wait for them to come to it.
            Player* summoner = ObjectAccessor::GetPlayer(*me, _summonerGUID);
            if (summoner && summoner->IsAlive() && summoner->IsWithinDist(me, SHADE_INTRO_ATTACK_RANGE))
                AttackStart(summoner);
        }

        if (!UpdateVictim())
        {
            // nobody is fighting it: don't stand at Tirion's door forever
            if (_idleTimer <= diff)
            {
                ProtectTirion(false);
                me->DespawnOrUnsummon();
            }
            else
                _idleTimer -= diff;

            return;
        }

        _idleTimer = SHADE_IDLE_DESPAWN_MS;

        // The core only leashes a creature that isn't being damaged. This stops it being kited
        // across the zone by a group that keeps hitting it.
        if (me->GetDistance(me->GetHomePosition()) > SHADE_LEASH_RANGE)
        {
            EnterEvadeMode(EVADE_REASON_BOUNDARY);
            return;
        }

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        // Frenzy lasts 5 minutes, so a long last 30% casts it again.
        if (me->HealthBelowPct(30) && !me->HasAura(SPELL_FRENZY))
        {
            if (!_saidFrenzy)
            {
                _saidFrenzy = true;
                Talk(SAY_SHADE_FRENZY);
            }

            DoCastSelf(SPELL_FRENZY, true);
        }

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    _events.Repeat(8s, 12s);
                    break;
                case EVENT_MORTAL_STRIKE:
                    DoCastVictim(SPELL_MORTAL_STRIKE);
                    _events.Repeat(12s, 16s);
                    break;
                case EVENT_SHADOW_BOLT_VOLLEY:
                    DoCastSelf(SPELL_SHADOW_BOLT_VOLLEY);
                    _events.Repeat(12s, 18s);
                    break;
                case EVENT_MEMORY:
                    Talk(SAY_SHADE_MEMORY);
                    _events.Repeat(20s, 30s);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    Creature* GetTirion() const
    {
        return _tirionGUID ? ObjectAccessor::GetCreature(*me, _tirionGUID) : nullptr;
    }

    void ProtectTirion(bool apply)
    {
        if (Creature* tirion = GetTirion())
            tirion->SetImmuneToNPC(apply);
    }

    EventMap _events;
    ObjectGuid _tirionGUID;
    ObjectGuid _summonerGUID;
    uint32 _introTimer = 0;
    uint32 _idleTimer = SHADE_IDLE_DESPAWN_MS;
    bool _saidFrenzy = false;
};

class LostLegendariesAshbringerCreatureScript : public AllCreatureScript
{
public:
    LostLegendariesAshbringerCreatureScript() : AllCreatureScript("LostLegendariesAshbringerCreatureScript") { }

    // Returning true means "handled": the core then skips its own gossip for this click.

    // Tirion's normal menu (his text and his quests), plus the line that starts the fight.
    bool CanCreatureGossipHello(Player* player, Creature* creature) override
    {
        if (!CanBeginCleansing(player, creature))
            return false;

        player->PrepareGossipMenu(creature, creature->GetGossipMenuId(), true);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I am ready, Tirion. Begin the cleansing.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_BEGIN_CLEANSING);
        player->SendPreparedGossip(creature);
        return true;
    }

    bool CanCreatureGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        // Options that come from the database have sender 0, so this can only be our line.
        if (creature->GetEntry() == NPC_TIRION_FORDRING && sender == GOSSIP_SENDER_MAIN && action == GOSSIP_ACTION_BEGIN_CLEANSING)
        {
            CloseGossipMenuFor(player);
            if (CanBeginCleansing(player, creature))
                BeginCleansing(creature, player);

            return true;
        }

        if (LostLegendaries::config.enabled && creature->GetEntry() == NPC_HIGH_INQUISITOR_FAIRBANKS
            && player->PlayerTalkClass->GetGossipMenu().GetMenuId() == GOSSIP_MENU_FAIRBANKS_LAST)
            player->AreaExploredOrEventHappens(QUEST_BLADE_OF_THE_HIGHLORD);  // does nothing unless the quest is in the log

        return false;
    }
};

class LostLegendariesAshbringerPlayerScript : public PlayerScript
{
public:
    LostLegendariesAshbringerPlayerScript() : PlayerScript("LostLegendariesAshbringerPlayerScript",
        { PLAYERHOOK_ON_BEFORE_QUEST_COMPLETE, PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST }) { }

    bool OnPlayerBeforeQuestComplete(Player* /*player*/, uint32 questId) override
    {
        return LostLegendaries::config.enabled || !IsAshbringerQuest(questId);
    }

    // Runs once the quest has been turned in and rewarded.
    void OnPlayerCompleteQuest(Player* player, Quest const* quest) override
    {
        if (!LostLegendaries::config.enabled || !LostLegendaries::config.announceAshbringer || quest->GetQuestId() != QUEST_THE_ASHBRINGER)
            return;

        sWorldSessionMgr->SendServerMessage(SERVER_MSG_STRING,
            Acore::StringFormat("{} has redeemed the Ashbringer!", player->GetName()));
    }
};

void AddLostLegendariesAshbringerScripts()
{
    RegisterCreatureAI(boss_lost_legendaries_mograine_shade);
    new LostLegendariesAshbringerCreatureScript();
    new LostLegendariesAshbringerPlayerScript();
}
