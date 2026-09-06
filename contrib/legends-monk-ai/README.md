# Legends MoP 5.4.8 Monk AI overlay

This directory is a staging overlay for
`Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8`.

It lives outside the active Chipa `mop-5.4.8-v2` source tree because that
branch uses a newer `mod-playerbots` layout. Copy these files into a true
fork of the Legends repository before build/PR validation.

## Target paths

Copy `modules/mod_playerbots/src/strategy/Classes/monk/*` to the same path in
the Legends fork and apply these patches at repository root in order:

1. `patches/AiFactory.cpp.patch` — include/activate `MonkAiObjectContext`
2. `patches/AiFactory.noncombat-monk.patch` — preserve Monk tank/dps/cure roles in the non-combat engine

The upstream build already contains Monk specialization and role detection in
`modules/mod_playerbots/src/AI/PlayerbotSpec.cpp` and `Factory/AiFactory.cpp`.
This overlay adds the missing Monk Strategy/Trigger/Action context and the
missing non-combat role branch.

## 5.4.8 / build 18414 spell validation

The rotation is written by spell name to match the existing Legends engine.
The following reference IDs validate the intended MoP spell/aura; no donor
implementation code is copied:

- Stance of the Sturdy Ox: 115069
- Stance of the Wise Serpent: 115070
- Stance of the Fierce Tiger: 103985
- Keg Smash: 121253
- Blackout Kick: 100784
- Tiger Palm: 100787
- Tiger Power aura: 125359
- Jab base spell: 100780 (MoP also has weapon/form variants)
- Purifying Brew: 119582
- Shuffle aura: 115307
- Guard: 115295
- Elusive Brew: 115308
- Provoke: 115546
- Rising Sun Kick: 107428
- Fists of Fury: 113656
- Touch of Karma: 122470
- Expel Harm: 115072
- Fortifying Brew: 115203
- Spear Hand Strike: 116705
- Spinning Crane Kick player cast: 101546
- Soothing Mist: 115175
- Surging Mist: 116694
- Enveloping Mist: 124682
- Renewing Mist cast: 115151; periodic aura/effect: 119611
- Uplift: 116670
- Life Cocoon: 116849
- Revival: 115310
- Thunder Focus Tea: 116680
- Mana Tea: 115294
- Detox: 115450
- Moderate Stagger aura: 124274
- Heavy Stagger aura: 124273

## Remaining validation

- Compile with `PLAYERBOTS=1` after moving the overlay into a real Legends fork.
- In-game test all three specs: party follow, tank threat/stagger handling,
  healing target selection, Windwalker rotation, interrupts and Detox.
- Keep feature and bug-fix commits separate.
