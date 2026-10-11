-- mod-lost-legendaries: Foror's Fabled Steed flies where flying is allowed.
--
-- Chromatic Mount (24576) is a ground mount. While it is up, the module's script also gives the
-- rider one of two server-only auras, picked by riding skill:
--   90230  +150% mounted flight speed (riding 225)
--   90231  +280% mounted flight speed (riding 300)
-- Each is a single SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED effect (207) on the caster, the
-- effect every flying mount spell has; the core turns flying on and off from it.
--   Attributes     passive, hidden, not logged, allowed while mounted (0x40|0x80|0x100|0x1000000)
--   AttributesEx4  only in flying areas (0x4000000), as on stock flying mounts, so the core keeps
--                  them to Outland and Northrend and removes them when the rider leaves
-- They exist only in spell_dbc. The client has no such spells and shows nothing for them.
-- Spell ids 90230-90239 are this module's range.
--
-- Idempotent. Undo with data/sql/uninstall/mod_lost_legendaries_uninstall_world.sql.

DELETE FROM `spell_dbc` WHERE `ID` IN (90230, 90231);
INSERT INTO `spell_dbc` (`ID`, `Attributes`, `AttributesEx4`, `CastingTimeIndex`, `ProcChance`, `DurationIndex`, `RangeIndex`, `EquippedItemClass`, `Effect_1`, `EffectDieSides_1`, `EffectBasePoints_1`, `ImplicitTargetA_1`, `EffectAura_1`, `SpellIconID`, `Name_Lang_enUS`, `Name_Lang_Mask`, `NameSubtext_Lang_Mask`, `Description_Lang_Mask`, `AuraDescription_Lang_Mask`, `EffectChainAmplitude_1`, `SchoolMask`, `EffectBonusMultiplier_1`, `EffectBonusMultiplier_2`, `EffectBonusMultiplier_3`) VALUES
(90230, 16777664, 67108864, 1, 101, 21, 1, -1, 6, 1, 149, 1, 207, 1, 'Chromatic Mount Flight', 16712190, 16712188, 16712190, 16712188, 1, 1, 1, 1, 1),
(90231, 16777664, 67108864, 1, 101, 21, 1, -1, 6, 1, 279, 1, 207, 1, 'Chromatic Mount Swift Flight', 16712190, 16712188, 16712190, 16712188, 1, 1, 1, 1, 1);

DELETE FROM `spell_script_names` WHERE `spell_id` = 24576 AND `ScriptName` = 'spell_lost_legendaries_chromatic_mount';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(24576, 'spell_lost_legendaries_chromatic_mount');
