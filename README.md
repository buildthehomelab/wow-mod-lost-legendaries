# Lost Legendaries

An [AzerothCore](https://www.azerothcore.org/) (WotLK 3.3.5a) module that puts lost legendary
items back in the game: items that are still in the game data but that nothing gives you.

| Legendary | Where it comes from |
| --- | --- |
| Talisman of Binding Shard | Baron Geddon, 3% |
| Talisman of Binding Fragment | Garr, 3% |
| Shard of the Defiler | Lord Kazzak, 3% |
| Glaive of the Defender | Sapphiron (level 60 Naxxramas), 3% |
| Ashbringer | A quest chain from Tirion Fordring that starts with the Corrupted Ashbringer |

The 3% is the chance of the **Bindings of the Windseeker**, and each drop has its own setting.
Every drop is its own roll, so it never takes the place of anything else the boss drops.

## Talisman of Binding

The **Talisman of Binding Shard** is a legendary neck that Blizzard took out of Baron Geddon's
loot early in vanilla. The item is still in the 3.3.5a data, together with an unused twin, the
**Talisman of Binding Fragment**. They drop in Molten Core, split across the same two bosses as
the two Bindings of the Windseeker: the Shard from Baron Geddon and the Fragment from Garr.

Both necks are item level 80 and need level 60: 13 Strength, 8 Stamina, 5 Agility, 24 Fire
Resistance and 24 Nature Resistance. The Shard also deals 4 Nature damage to anyone who hits you
in melee. In stock AzerothCore the Fragment has no such effect;
[mod-individual-progression](https://github.com/ZhengPeiRu21/mod-individual-progression) gives it
the same one.

The only change to the items themselves: both carry the "deprecated" flag Blizzard put on them
when it retired them, which marks an item as unusable. The module clears it.

## Shard of the Defiler

A legendary dagger that was never obtainable: item level 70, 52-61 damage plus 10-15 Shadow
damage at speed 1.10. While it's equipped you become an **Echo of Archimonde**, with 15% melee
haste and 15% critical strike chance. The Defiler is Archimonde, so it drops from the demon who
leads what is left of his army on Azeroth, **Lord Kazzak** in the Blasted Lands.

This module doesn't change the item.

## Glaive of the Defender

A warrior-only legendary: 138-210 damage at speed 2.30, 80 Armor, 18 Stamina, 12 Strength,
12 Agility, 8 Frost and 8 Shadow Resistance, 16 defense rating, and a chance on hit to cast
Shield Wall on you. It drops from **Sapphiron** in the level 60 Naxxramas, whose Frost Aura and
Life Drain are what those two resistances are for.

It was a **Main Hand sword** in vanilla. The 3.3.5a data has it as a bow in the ranged slot, so
the module turns it back into a Main Hand sword. Stats, effects and model are unchanged. Players
need to clear their client's `Cache` folder to see the corrected tooltip.

The chance-on-hit effect is the warrior's own Shield Wall spell. It can only trigger while your
Shield Wall is off cooldown, and when it does, Shield Wall goes on cooldown as if you had pressed
it. As a bow the effect could never trigger, so this is new behavior that comes with the fix.

The level 60 Sapphiron comes from mod-individual-progression's Naxxramas 40. Without that module
the Glaive still gets its fix, but nothing drops it.

## Ashbringer

**Ashbringer** is in the game data as a legendary two-hand sword (201-247 damage plus 30-50 Holy
damage, with a chance on hit to blast the target for 700 Fire damage), attached to an unused
quest that no one gives. Here it's the reward of a five-quest chain that redeems the
**Corrupted Ashbringer**, the Four Horsemen's drop in the level 60 Naxxramas.

Tirion Fordring, at his hovel in the Eastern Plaguelands, gives every step to both factions. He
only offers the first one to a level 60 player who carries a Corrupted Ashbringer.

| # | Quest | What you do |
| --- | --- | --- |
| 1 | The Blade of the Scarlet Highlord | Walk into the Scarlet Monastery's Cathedral with the Corrupted Ashbringer equipped. The Crusade kneels, and High Inquisitor Fairbanks (behind the hidden wall) will talk to you. Hear his tale to the end. |
| 2 | The Hands Behind the Betrayal | Kill Balnazzar in Stratholme and Kel'Thuzad in Naxxramas. |
| 3 | A Blade Made New | Bring 20 Righteous Orbs, 10 Arcanite Bars and 5 Stratholme Holy Water. |
| 4 | What the Blade Remembers | Tell Tirion you are ready, then defeat the Shade of Highlord Mograine, which he drives out of the blade. Bring a group. |
| 5 | The Ashbringer | Hand the Corrupted Ashbringer to Tirion. He gives you Ashbringer in its place. |

Step 1 uses the Corrupted Ashbringer event that is already in AzerothCore; the module only
gives quest credit when you reach Fairbanks' last line. Finishing step 5 is announced to the whole
server (`LostLegendaries.Ashbringer.Announce`).

### The Shade of Highlord Mograine

While you have step 4, Tirion's dialogue has one more line: "I am ready, Tirion. Begin the
cleansing." Nothing happens until someone picks it, so you can gather the raid first. Tirion then
begins, and five seconds later the shade appears in front of him, at his hovel in the open world,
and goes for the player who asked. It is a level 63 boss with about 400,000 health, tuned like
Prince Thunderaan from the Thunderfury chain, so it wants a raid group of around ten rather than
forty.

| Ability | What it does |
| --- | --- |
| Cleave | 110% melee damage to its target and the players next to them. |
| Mortal Strike | 200% weapon damage, and healing on the target is halved for 5 seconds. |
| Shadow Bolt Volley | About 700 Shadow damage to everyone within 30 yards. |
| Frenzy | Below 30% health: 25% more damage and 40% faster attacks until it dies. |

It speaks in the Corrupted Ashbringer's own whispers. When it dies, every player within 100 yards
who has the quest gets the credit, whoever hit it first. It has no loot: the reward is the sword.

Fight it where it appears. The shade vanishes if the group wipes, if nobody fights it for five
minutes, or if it is led away: like any open-world creature it gives up 30 yards from where it
appeared unless it is being damaged, and it never goes further than 80. Pick Tirion's line again
to call it back at full health. Tirion can't be attacked by NPCs while a shade is up, so the
fight can't kill your quest giver.

The chain needs mod-individual-progression's Naxxramas 40, for the Corrupted Ashbringer and for
the level 60 Kel'Thuzad. Without it Tirion never offers the first quest.

## Patch Notes: Lost Legendaries

Category: Dungeons

- **Baron Geddon** now has a small chance to drop the **Talisman of Binding Shard**, a legendary
  necklace last seen in the earliest days of Molten Core. **Garr** has the same chance to drop
  its twin, the **Talisman of Binding Fragment**.
- **Lord Kazzak** now has a small chance to drop the **Shard of the Defiler**, a legendary dagger.
- **Sapphiron** now has a small chance to drop the **Glaive of the Defender**, a legendary sword
  for warriors. The Glaive is a Main Hand sword again instead of a bow.
- **Tirion Fordring** now recognizes the **Corrupted Ashbringer**. Bring it to him in the Eastern
  Plaguelands to begin a quest chain that ends with the **Ashbringer** itself.
- The chain's last trial is a new boss, the **Shade of Highlord Mograine**, which must be defeated
  at Tirion's side before the blade can be cleansed.

> These items have been sitting in the game's data since vanilla with no way to get them. They
> are meant to stay rare: the drops use the same odds as the Bindings of the Windseeker.

## Requirements

- [AzerothCore](https://www.azerothcore.org/) wotlk (master) and a WoW 3.3.5a (12340) client.
- No client patch.
- [mod-individual-progression](https://github.com/ZhengPeiRu21/mod-individual-progression) for
  the Glaive of the Defender's drop and the Ashbringer chain (both need its Naxxramas 40). The
  other three drops work without it, as long as your server has Lord Kazzak spawned. Without it
  the worldserver logs one quest error at startup, for the Kel'Thuzad kill that can't exist.

## Installation

Clone it into your AzerothCore `modules` folder, **as `mod-lost-legendaries`**. AzerothCore
derives the module's loader name from the folder name:

```bash
cd azerothcore-wotlk/modules
git clone https://github.com/buildthehomelab/wow-mod-lost-legendaries.git mod-lost-legendaries
```

Re-run CMake, rebuild the worldserver, and copy `conf/mod_lost_legendaries.conf.dist` to
`mod_lost_legendaries.conf` in your config directory. The world database updates are applied
automatically on the next worldserver start.

To check that it's loaded, look for this line in the worldserver log at startup:

```
mod-lost-legendaries: enabled, drop chances: Talisman of Binding 3%, Shard of the Defiler 3%, Glaive of the Defender 3%
```

## Configuration

| Setting | Default | What it does |
| --- | --- | --- |
| `LostLegendaries.Enable` | `1` | Master switch. With `0` the lost legendaries never drop and the Ashbringer quests can't be completed. |
| `LostLegendaries.TalismanOfBinding.Chance` | `3` | Drop chance in percent for each Talisman of Binding, per kill. `0` stops both dropping. |
| `LostLegendaries.ShardOfTheDefiler.Chance` | `3` | Drop chance in percent for the Shard of the Defiler. `0` stops it dropping. |
| `LostLegendaries.GlaiveOfTheDefender.Chance` | `3` | Drop chance in percent for the Glaive of the Defender. `0` stops it dropping. |
| `LostLegendaries.Ashbringer.Announce` | `1` | Announce to the whole server when a player finishes the Ashbringer chain. |

The core multiplies each chance by `Rate.Drop.Item.Legendary`, as it does for the Bindings of
the Windseeker. A chance of `100` always drops, whatever the rate.

## What it changes in the database

- Four rows in `creature_loot_template` (Baron Geddon, Garr, Lord Kazzak, and the level 60
  Sapphiron if it exists). The chance isn't read from those rows: the module sets it on every
  roll from the config, and makes the roll fail when the module is switched off. The module also
  checks the four rows at every startup and adds back any that are missing, because another
  module's SQL can remove them (mod-individual-progression rebuilds its Naxxramas loot whenever
  its own SQL changes). It logs a warning when it does.
- The "deprecated" flag is cleared on the two Talismans of Binding in `item_template`.
- The Glaive of the Defender's type, slot, material and sheath in `item_template`, plus an
  `item_dbc` row with the same values. The core takes those fields from Item.dbc, so without the
  `item_dbc` row it would undo the change at startup.
- Quests 9501300-9501304 with their texts, Tirion Fordring as their giver and ender, and one
  `conditions` row for the first quest.
- Creature 9501300, the Shade of Highlord Mograine, with its model, weapon and lines. It has no
  spawn: the module's script summons it.
- Ids 9501300-9501399 are this module's range.

## Uninstalling

Remove the module folder and rebuild, then run
`data/sql/uninstall/mod_lost_legendaries_uninstall_world.sql` on the world database by hand.
Do both: with the module gone and its rows still in the database, the items keep dropping at the
3% written in their loot rows, and Tirion still offers the Ashbringer chain even though its first
and fourth steps can no longer be finished. Items that players already own are left alone.

## Troubleshooting

- **An item never drops:** at 3% that is the normal result of most kills. To test, set its
  `Chance` to `100`, reload the config and kill the boss. If it still doesn't drop, check for the
  `mod-lost-legendaries: enabled` line in the worldserver log and for the item's row in
  `creature_loot_template`.
- **The Glaive of the Defender still says "Bow" or goes in the ranged slot:** clear the client's
  `Cache` folder. If the worldserver log says the item "has wrong Subclass value", the `item_dbc`
  row is missing.
- **The Glaive of the Defender sits oddly on the character:** the client's own Item.dbc still
  lists it as a bow, and the client uses that file to place the model. The tooltip and the slot
  come from the server and are correct.
- **Tirion doesn't offer the first quest:** you need to be level 60 and have a Corrupted
  Ashbringer on you, and the server needs mod-individual-progression's Naxxramas 40.
- **No credit from Fairbanks:** he only talks during the Corrupted Ashbringer event, which needs
  the sword equipped when you enter the Cathedral. Keep choosing the dialogue options until he
  tells you where Mograine's other son is.
- **No credit for Balnazzar or Kel'Thuzad:** the kills only count while you have the second quest.
- **The shade didn't appear, or it vanished:** talk to Tirion while you have the fourth quest and
  no shade is up, and pick "I am ready, Tirion. Begin the cleansing." It appears five seconds
  after he speaks. It vanishes when it is led more than 30 yards from that spot without being
  damaged.
- **The module doesn't build or load:** the folder must be named `mod-lost-legendaries`, because
  AzerothCore derives the loader name from it. Re-run CMake after cloning.

## Credits

Author: [buildthehomelab](https://github.com/buildthehomelab)

## License

MIT, see [LICENSE](LICENSE).
