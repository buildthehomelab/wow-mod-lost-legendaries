-- mod-lost-legendaries: the Talisman of Binding necks drop in Molten Core.
--
-- Baron Geddon (12056) - Talisman of Binding Shard (17782)
-- Garr (12057)         - Talisman of Binding Fragment (17783)
--
-- The two legendary necks are split across the same two bosses as the Bindings of the
-- Windseeker (18563 / 18564) and use the same 3% chance. Each is its own roll (GroupId 0), so
-- it never replaces the Bindings or any other loot. Stock AzerothCore has no source for either
-- neck.
-- The 3% here is only what the row says: while the module is loaded,
-- LostLegendaries.TalismanOfBinding.Chance decides the real chance. Idempotent.
-- Undo with data/sql/uninstall/mod_lost_legendaries_uninstall_world.sql.

DELETE FROM `creature_loot_template`
WHERE (`Entry` = 12056 AND `Item` = 17782)
   OR (`Entry` = 12057 AND `Item` = 17783);
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(12056, 17782, 0, 3, 0, 1, 0, 1, 1, 'Baron Geddon - Talisman of Binding Shard'),
(12057, 17783, 0, 3, 0, 1, 0, 1, 1, 'Garr - Talisman of Binding Fragment');

-- Both necks carry item flag 16 (deprecated: "cannot equip or use"), which Blizzard set when it
-- retired them. The server ignores the flag, but an item that drops should not be marked
-- unusable for the client. Nothing else about the items changes. Players who already have one
-- cached need to clear their Cache folder.
UPDATE `item_template` SET `Flags` = `Flags` & ~16 WHERE `entry` IN (17782, 17783);
