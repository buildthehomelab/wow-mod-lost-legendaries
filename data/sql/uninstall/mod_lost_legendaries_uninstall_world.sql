-- mod-lost-legendaries: undo the module's world database changes. Run it by hand on the world
-- database after removing the module. It isn't applied automatically: AzerothCore only runs the
-- module's db-world folder. Remove the module first: while it is still built in, its startup
-- check puts the loot rows back.
--
-- Removes the five loot rows, the Ashbringer quest chain and its summoned boss, turns the Glaive
-- of the Defender back into the ranged-slot bow that stock AzerothCore has, marks the two
-- Talismans of Binding deprecated again, and makes Foror's Fabled Steed teach its spell again. Items that players already own are left alone, and so are
-- their finished quests. It also forgets that the module's updates were applied, so installing
-- the module again applies them again. Idempotent: safe to run again.

-- Talisman of Binding Shard / Fragment, Shard of the Defiler, Glaive of the Defender, Foror's
-- Fabled Steed
DELETE FROM `creature_loot_template`
WHERE (`Entry` = 12056 AND `Item` = 17782)
   OR (`Entry` = 12057 AND `Item` = 17783)
   OR (`Entry` = 12397 AND `Item` = 17142)
   OR (`Entry` = 351018 AND `Item` = 23051)
   OR (`Entry` = 11583 AND `Item` = 20221);

-- Foror's Fabled Steed: the stock item, which teaches Chromatic Mount and is consumed
UPDATE `item_template` SET
    `description` = 'Teaches you how to summon this mount.  This is a very fast mount.',
    `spellid_1` = 55884, `spelltrigger_1` = 0, `spellcharges_1` = -1,
    `spellid_2` = 24576, `spelltrigger_2` = 6
WHERE `entry` = 20221;

-- Talisman of Binding Shard / Fragment: the deprecated flag again
UPDATE `item_template` SET `Flags` = `Flags` | 16 WHERE `entry` IN (17782, 17783);

-- Glaive of the Defender: stock type, slot, material and sheath
DELETE FROM `item_dbc` WHERE `ID` = 23051;
UPDATE `item_template` SET `subclass` = 2, `InventoryType` = 15, `Material` = 2, `sheath` = 0 WHERE `entry` = 23051;

-- Ashbringer quest chain
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 19 AND `SourceEntry` = 9501300;
DELETE FROM `creature_queststarter` WHERE `quest` BETWEEN 9501300 AND 9501304;
DELETE FROM `creature_questender` WHERE `quest` BETWEEN 9501300 AND 9501304;
DELETE FROM `quest_offer_reward` WHERE `ID` BETWEEN 9501300 AND 9501304;
DELETE FROM `quest_request_items` WHERE `ID` BETWEEN 9501300 AND 9501304;
DELETE FROM `quest_template_addon` WHERE `ID` BETWEEN 9501300 AND 9501304;
DELETE FROM `quest_template` WHERE `ID` BETWEEN 9501300 AND 9501304;

-- Shade of Highlord Mograine
DELETE FROM `creature_text` WHERE `CreatureID` = 9501300;
DELETE FROM `creature_equip_template` WHERE `CreatureID` = 9501300;
DELETE FROM `creature_template_model` WHERE `CreatureID` = 9501300;
DELETE FROM `creature_template` WHERE `entry` = 9501300;

-- Let a later reinstall apply the module's updates again
DELETE FROM `updates` WHERE `name` LIKE 'mod\_lost\_legendaries\_%';
