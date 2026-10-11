-- mod-lost-legendaries: Foror's Fabled Steed.
--
-- Nefarian (11583) - Foror's Fabled Steed (20221)
--
-- A legendary mount item that stock AzerothCore has no source for. Its spell, Chromatic Mount
-- (24576), is a very fast ground mount on a chromatic drake, and Nefarian is the one who bred the
-- chromatic dragonflight. The row is its own roll (GroupId 0), so it never replaces other loot.
-- 1% is what the row says, the same as the Deathcharger's Reins: while the module is loaded,
-- LostLegendaries.FororsFabledSteed.Chance decides the real chance.
--
-- The 3.3.5a spell data mounts the player on a bronze drake (creature 18768). The module's script
-- points the spell back at creature 15135 "Chromatic Drake Mount", which is stock and has the
-- chromatic look (the same skin as Gyth in Upper Blackrock Spire). Nothing in the database changes
-- for that.
--
-- Idempotent. Undo with data/sql/uninstall/mod_lost_legendaries_uninstall_world.sql.

DELETE FROM `creature_loot_template` WHERE `Entry` = 11583 AND `Item` = 20221;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(11583, 20221, 0, 1, 0, 1, 0, 1, 1, 'Nefarian - Foror''s Fabled Steed');

-- The item becomes one you keep and use to mount, as mounts worked in vanilla, instead of one
-- that teaches the spell and disappears. The 3.3.5a client only lists a mount in the Mounts tab
-- when its spell is in the client's own SkillLineAbility.dbc, and Chromatic Mount is not: a
-- player who learned it would have no button for it.
-- Stock: spell 1 is 55884 "Learning" (on use, consumed) and spell 2 is 24576 (learned).
-- Now: spell 1 is 24576 on use, never consumed, on the usual mount cooldown category (330).
-- Level, riding skill, binding, quality and name are untouched. Players who already have the
-- item cached need to clear their Cache folder.
UPDATE `item_template` SET
    `description` = '',
    `spellid_1` = 24576, `spelltrigger_1` = 0, `spellcharges_1` = 0,
    `spellid_2` = 0, `spelltrigger_2` = 0
WHERE `entry` = 20221;
