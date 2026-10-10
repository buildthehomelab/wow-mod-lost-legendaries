-- mod-lost-legendaries: Shard of the Defiler and Glaive of the Defender.
--
-- Lord Kazzak (12397)              - Shard of the Defiler (17142)
-- Sapphiron, Naxxramas 40 (351018) - Glaive of the Defender (23051)
--
-- Both are stock legendary items with no source in stock AzerothCore. Each is its own 3% roll
-- (GroupId 0), so it never replaces other loot. The 3% is only what the row says: while the module
-- is loaded, LostLegendaries.ShardOfTheDefiler.Chance and LostLegendaries.GlaiveOfTheDefender.Chance
-- decide the real chance. Idempotent.
-- Undo with data/sql/uninstall/mod_lost_legendaries_uninstall_world.sql.

-- Shard of the Defiler: the Defiler is Archimonde, and Kazzak leads what is left of his army.
DELETE FROM `creature_loot_template` WHERE `Entry` = 12397 AND `Item` = 17142;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(12397, 17142, 0, 3, 0, 1, 0, 1, 1, 'Lord Kazzak - Shard of the Defiler');

-- Glaive of the Defender: its Frost and Shadow Resistance match Sapphiron's Frost Aura and Life
-- Drain. The level 60 Sapphiron (351018) comes from mod-individual-progression's Naxxramas 40, so
-- the row is only added when that creature exists. Without it the Glaive stays unobtainable.
-- If this runs before that module's SQL, or that module rebuilds its Naxxramas loot later, the
-- row is missing; the module's startup check adds it again.
DELETE FROM `creature_loot_template` WHERE `Entry` = 351018 AND `Item` = 23051;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`)
SELECT 351018, 23051, 0, 3, 0, 1, 0, 1, 1, 'Sapphiron (Naxxramas 40) - Glaive of the Defender'
FROM `creature_template` WHERE `entry` = 351018 AND `lootid` = 351018;

-- Glaive of the Defender is a Main Hand sword again. The 3.3.5a data has it as a bow in the
-- ranged slot (class 2, subclass 2, InventoryType 15, no sheath, wood). Stats, effects and model
-- are unchanged. The sheath value is the Warglaives of Azzinoth's (1, on the back).
-- The core takes class, subclass, slot, material and sheath from Item.dbc and ignores
-- item_template where they differ (DBC.EnforceItemAttributes), so the item_dbc row carries the
-- same change. The client's own Item.dbc still says "bow": players need to clear their Cache
-- folder to see the new tooltip.
DELETE FROM `item_dbc` WHERE `ID` = 23051;
INSERT INTO `item_dbc` (`ID`, `ClassID`, `SubclassID`, `Sound_Override_Subclassid`, `Material`, `DisplayInfoID`, `InventoryType`, `SheatheType`) VALUES
(23051, 2, 7, -1, 1, 35450, 21, 1);

UPDATE `item_template` SET `subclass` = 7, `InventoryType` = 21, `Material` = 1, `sheath` = 1 WHERE `entry` = 23051;
