-- mod-lost-legendaries: the Ashbringer quest chain.
--
-- Ashbringer (13262) is a stock legendary two-hand sword whose only source in stock AzerothCore is
-- an unused quest that no one gives. This chain turns the Corrupted Ashbringer (22691, from the
-- Four Horsemen in mod-individual-progression's Naxxramas 40) into it.
--
--   9501300 The Blade of the Scarlet Highlord  hear High Inquisitor Fairbanks' tale (Scarlet Monastery)
--   9501301 The Hands Behind the Betrayal      kill Balnazzar (Stratholme) and Kel'Thuzad (Naxxramas 40)
--   9501302 A Blade Made New                   20 Righteous Orbs, 10 Arcanite Bars, 5 Stratholme Holy Water
--   9501303 What the Blade Remembers           defeat the Shade of Highlord Mograine (summoned boss)
--   9501304 The Ashbringer                     hand over the Corrupted Ashbringer, receive Ashbringer
--
-- Tirion Fordring (1855, Eastern Plaguelands, talks to both factions) gives and ends every step.
-- The first quest only shows while the player carries a Corrupted Ashbringer. Its objective is an
-- event: the module credits it when the player reaches the end of Fairbanks' dialogue (gossip menu
-- 7268), which Fairbanks only offers during the stock Corrupted Ashbringer event in the Cathedral.
-- The two kill quests are typed Raid (62) because the core gives no quest credit in a raid group
-- otherwise; the rest are typed Legendary (83).
--
-- The Shade of Highlord Mograine (creature 9503000) is summoned by the module's script when a
-- player on the fourth quest tells Tirion to begin. It is tuned like Prince Thunderaan, the
-- Thunderfury chain's summoned boss: level 63, boss rank, about 400,000 health. It uses Highlord
-- Mograine's model (16139) and weapon (22709), and the crowd-control immunity set that stock
-- AzerothCore gives him (-406). It has no loot.
-- type_flags stays 0 on purpose: the boss flag (4) makes the core treat it as a world boss, and
-- an open-world world boss evades as soon as it is 30 yards from where it appeared, even in the
-- middle of a fight. Without the flag it can be moved for as long as it is being damaged.
--
-- This module's ids: quests 9501300-9501399, creatures 9503000-9503099. No stock rows are changed.
-- Idempotent. Undo with data/sql/uninstall/mod_lost_legendaries_uninstall_world.sql.

-- ---------------------------------------------------------------------------------------------
-- Shade of Highlord Mograine
-- ---------------------------------------------------------------------------------------------

-- Until 2026-10-11 the shade was creature 9501300. That id is inside 9501000-9501499, the block the
-- druid form look pack numbers its preview creatures from, so the shade moved to 9503000. Remove
-- the old rows, but only while 9501300 is still the shade: by now it may be someone else's.
DELETE x FROM `creature_text` x JOIN `creature_template` t ON t.`entry` = x.`CreatureID`
WHERE t.`entry` = 9501300 AND t.`ScriptName` = 'boss_lost_legendaries_mograine_shade';
DELETE x FROM `creature_equip_template` x JOIN `creature_template` t ON t.`entry` = x.`CreatureID`
WHERE t.`entry` = 9501300 AND t.`ScriptName` = 'boss_lost_legendaries_mograine_shade';
DELETE x FROM `creature_template_model` x JOIN `creature_template` t ON t.`entry` = x.`CreatureID`
WHERE t.`entry` = 9501300 AND t.`ScriptName` = 'boss_lost_legendaries_mograine_shade';
DELETE FROM `creature_template` WHERE `entry` = 9501300 AND `ScriptName` = 'boss_lost_legendaries_mograine_shade';

DELETE FROM `creature_template` WHERE `entry` = 9503000;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `exp`, `faction`, `speed_walk`, `speed_run`, `detection_range`, `rank`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags2`, `type`, `type_flags`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `ExperienceModifier`, `RegenHealth`, `CreatureImmunitiesId`, `flags_extra`, `ScriptName`) VALUES
(9503000, 'Shade of Highlord Mograine', 'The Ashbringer', 63, 63, 0, 21, 1, 1.71429, 20, 3, 13, 2000, 2000, 1, 1, 1, 2048, 6, 0, '', 0, 1, 120, 1, 1.3, 0, 1, -406, 0, 'boss_lost_legendaries_mograine_shade');

DELETE FROM `creature_template_model` WHERE `CreatureID` = 9503000;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(9503000, 0, 16139, 1, 1, 0);

DELETE FROM `creature_equip_template` WHERE `CreatureID` = 9503000;
INSERT INTO `creature_equip_template` (`CreatureID`, `ID`, `ItemID1`, `ItemID2`, `ItemID3`, `VerifiedBuild`) VALUES
(9503000, 1, 22709, 0, 0, 0);

-- The lines are the Corrupted Ashbringer's own whispers, with their stock sounds.
DELETE FROM `creature_text` WHERE `CreatureID` = 9503000;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(9503000, 0, 0, 'I was once called Ashbringer.', 14, 0, 100, 0, 0, 8908, 0, 0, 'Shade of Highlord Mograine - summoned'),
(9503000, 1, 0, 'Betrayed by my order.', 14, 0, 100, 0, 0, 8920, 0, 0, 'Shade of Highlord Mograine - memory'),
(9503000, 1, 1, 'Destroyed by Kel''Thuzad.', 14, 0, 100, 0, 0, 8921, 0, 0, 'Shade of Highlord Mograine - memory'),
(9503000, 1, 2, 'Made to serve.', 14, 0, 100, 0, 0, 8922, 0, 0, 'Shade of Highlord Mograine - memory'),
(9503000, 1, 3, 'My son watched me die.', 14, 0, 100, 0, 0, 8923, 0, 0, 'Shade of Highlord Mograine - memory'),
(9503000, 2, 0, 'Kill them all!', 14, 0, 100, 0, 0, 8928, 0, 0, 'Shade of Highlord Mograine - frenzy at 30%'),
(9503000, 3, 0, 'I was pure once...', 14, 0, 100, 0, 0, 8906, 0, 0, 'Shade of Highlord Mograine - death');

-- ---------------------------------------------------------------------------------------------
-- Quests
-- ---------------------------------------------------------------------------------------------

DELETE FROM `quest_template` WHERE `ID` BETWEEN 9501300 AND 9501304;
INSERT INTO `quest_template` (`ID`, `QuestType`, `QuestLevel`, `MinLevel`, `QuestSortID`, `QuestInfoID`, `SuggestedGroupNum`, `RewardNextQuest`, `RewardItem1`, `RewardAmount1`, `LogTitle`, `LogDescription`, `QuestDescription`, `AreaDescription`, `QuestCompletionLog`, `RequiredNpcOrGo1`, `RequiredNpcOrGo2`, `RequiredNpcOrGoCount1`, `RequiredNpcOrGoCount2`, `RequiredItemId1`, `RequiredItemId2`, `RequiredItemId3`, `RequiredItemCount1`, `RequiredItemCount2`, `RequiredItemCount3`, `VerifiedBuild`) VALUES
(9501300, 2, 60, 60, 139, 83, 0, 9501301, 0, 0,
 'The Blade of the Scarlet Highlord',
 'Enter the Cathedral of the Scarlet Monastery with the Corrupted Ashbringer equipped and hear High Inquisitor Fairbanks'' tale to its end. Then return to Tirion Fordring in the Eastern Plaguelands.',
 'Where did you... no. Do not answer. I would know that blade if I were blind.$B$BAlexandros Mograine carried it against the Scourge when I was already an exile, and the dead burned to ash before him. The Ashbringer. That it is black now, and that it whispers, tells me how his story ended.$B$BIt does not tell me who ended it. Alexandros was murdered, $N, and one man saw it done. Fairbanks was his advisor. They say the Crusade killed him for speaking of it and walled him up in a chamber off their own cathedral.$B$BGo there with the blade in your hands. The dead will know it.',
 'Hear High Inquisitor Fairbanks'' tale',
 'Return to Tirion Fordring in the Eastern Plaguelands.',
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(9501301, 2, 60, 60, 139, 62, 40, 9501302, 0, 0,
 'The Hands Behind the Betrayal',
 'Kill Balnazzar in Stratholme and Kel''Thuzad in Naxxramas. Then return to Tirion Fordring in the Eastern Plaguelands.',
 'Two hands took Alexandros from the Light, $N.$B$BThe first is Balnazzar''s. The dreadlord wears the Grand Crusader''s face in Stratholme, and it was he who turned Renault against his own father.$B$BThe second is Kel''Thuzad''s. The lich raised Mograine to ride as one of his Horsemen and chained what was left of the man to that blade. You have already put the Highlord''s body to rest, or you would not be holding his sword. His soul will not follow while the lich endures.$B$BDestroy them both. While either one stands, anything I do to this blade will be undone.',
 '',
 'Return to Tirion Fordring in the Eastern Plaguelands.',
 10813, 351019, 1, 1, 0, 0, 0, 0, 0, 0, 0),
(9501302, 2, 60, 60, 139, 83, 0, 9501303, 0, 0,
 'A Blade Made New',
 'Bring 20 Righteous Orbs, 10 Arcanite Bars and 5 vials of Stratholme Holy Water to Tirion Fordring in the Eastern Plaguelands.',
 'The Ashbringer was not always a sword. It began as a crystal, as dark as this blade is now, until so much of the Light was poured into it that it could hold no more shadow. I mean to do the same again.$B$BI will need Righteous Orbs. The Crusade''s zeal was twisted, but the Light they gathered into those orbs was real. I will need holy water from the chapels of Stratholme, where he fell. And I will need arcanite, to hold the steel together while the corruption burns out of it.$B$BBring me these, $N, and keep the blade close until you do.',
 '',
 'Return to Tirion Fordring in the Eastern Plaguelands.',
 0, 0, 0, 0, 12811, 12360, 13180, 20, 10, 5, 0),
(9501303, 2, 60, 60, 139, 62, 10, 9501304, 0, 0,
 'What the Blade Remembers',
 'Tell Tirion Fordring when you are ready, then defeat the Shade of Highlord Mograine beside him in the Eastern Plaguelands. If it defeats you, speak to Tirion to call it back.',
 'Then it is time.$B$BI will call the Light into the blade, and the Light does not share. Whatever Kel''Thuzad left in this steel will be driven out, and it will not go quietly. It knows only one shape, $N, and it will wear his.$B$BThis is no fight for one sword. Gather those you trust and have them stand close.$B$BWhen you tell me you are ready, I will begin. If it drives you back, come and speak to me, and we will try again.',
 '',
 'Return to Tirion Fordring in the Eastern Plaguelands.',
 9503000, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0),
(9501304, 2, 60, 60, 139, 83, 0, 0, 13262, 1,
 'The Ashbringer',
 'Give the Corrupted Ashbringer to Tirion Fordring.',
 'I am no smith, and no king''s forge stands behind me. I have only what I have always had.$B$BGive me the blade. Alexandros Mograine has waited long enough to see it clean.$B$BAnd $N... if this works, it will not be mine to carry. It answered to you in the dark. Let it answer to you in the Light.',
 '',
 'Return to Tirion Fordring in the Eastern Plaguelands.',
 0, 0, 0, 0, 22691, 0, 0, 1, 0, 0, 0);

-- SpecialFlags 2: the first quest's objective is an event, credited from the module's script.
DELETE FROM `quest_template_addon` WHERE `ID` BETWEEN 9501300 AND 9501304;
INSERT INTO `quest_template_addon` (`ID`, `PrevQuestID`, `SpecialFlags`) VALUES
(9501300, 0, 2),
(9501301, 9501300, 0),
(9501302, 9501301, 0),
(9501303, 9501302, 0),
(9501304, 9501303, 0);

DELETE FROM `quest_request_items` WHERE `ID` BETWEEN 9501300 AND 9501304;
INSERT INTO `quest_request_items` (`ID`, `CompletionText`, `VerifiedBuild`) VALUES
(9501300, 'The blade still whispers. What did Fairbanks tell you?', 0),
(9501301, 'A dreadlord and a lich. I do not ask it lightly, $N. But the blade will not be free while they stand.', 0),
(9501302, 'Have you gathered what I asked for?', 0),
(9501303, 'It must be driven out, $N. There is no other way through this.', 0),
(9501304, 'Hand me the blade, $N.', 0);

DELETE FROM `quest_offer_reward` WHERE `ID` BETWEEN 9501300 AND 9501304;
INSERT INTO `quest_offer_reward` (`ID`, `RewardText`, `VerifiedBuild`) VALUES
(9501300, 'His own son. And a dreadlord''s hand behind the boy.$B$BFairbanks says the blade is beyond saving. He died in the dark believing it, and I will not fault him for that. But I have seen the Light forgive what men could not, $N. I am not ready to bury this sword.', 0),
(9501301, 'It is quieter. Do you hear it? The whispering has not stopped, but there is only one voice in it now, and it is his.$B$BNow we may begin.', 0),
(9501302, 'This will serve. Set it all down by the fire.$B$BRest while you can, $N. What comes next will not be gentle.', 0),
(9501303, 'Listen.$B$BNothing. For the first time since Stratholme, the blade is silent.$B$BHe is free, $N. Now let us see to his sword.', 0),
(9501304, '<Tirion closes his eyes. The blade shakes in his hands, and for a moment you hear a voice that is not his.>$B$B...Forgiven...$B$B<The black falls from the steel like ash.>$B$BIt is done. Take it, $N. The Scourge once feared this sword more than any army. Teach them to fear it again.', 0);

-- Tirion Fordring gives and ends every step.
DELETE FROM `creature_queststarter` WHERE `quest` BETWEEN 9501300 AND 9501304;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(1855, 9501300),
(1855, 9501301),
(1855, 9501302),
(1855, 9501303),
(1855, 9501304);

DELETE FROM `creature_questender` WHERE `quest` BETWEEN 9501300 AND 9501304;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(1855, 9501300),
(1855, 9501301),
(1855, 9501302),
(1855, 9501303),
(1855, 9501304);

-- The first quest is only offered to a player who has a Corrupted Ashbringer (equipped or in bags).
-- That is also what keeps the chain out of reach on a server without mod-individual-progression's
-- Naxxramas 40: nothing else drops the Corrupted Ashbringer.
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 19 AND `SourceEntry` = 9501300;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(19, 0, 9501300, 0, 0, 2, 0, 22691, 1, 0, 0, 0, 0, '', 'The Blade of the Scarlet Highlord - player has a Corrupted Ashbringer');
