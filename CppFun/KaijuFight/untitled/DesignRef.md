---
---
---
# KAIJU FIGHT GDD
---
# BY PETER DORIA
---
---
---

# Pitch

You find an early 90's military console that has not been turned on in many years. 

---
# Combat Design - Real-time Combat Puzzle
---

**DO'S / DON'TS**
- NO COMPLEX COMBOS TO DO A MOVE. EMPHASIS ON PUZZLE STRATEGY OVER INPUT CHALLENGE
- All monsters have the same max energy. Easier to balance. Easier for players to understand (may break this for fun monster)
- SCREW BALANCE
- No Turn-based - nah. dunno how to make that in a way that isn't pokemon, and I don't want to make pokemon. Kinda sucks.
   * Puzzle with chance element (elements + dice) tech help

**Design ?'s**
- Game Speed is directly related to kaiju speed. How slow? How fast?
- Key placement affects feeling for each kaiju
- Playing a kaiju with all inputs in one place vs. across the keyboard - Feel like a spider/master controller vs. a league character)
- Fight Length 2-5 minutes? Lumbering kaiju. Should feel like controlling massive beasts where the fight sways back and forth.
- But will that be too slow? Will that be fun? The fight needs to be able to turn quickly to have uncertainty to be fun.
- Range is fixed for each ability, VISUAL? Or just let the player miss? An indicator when distance is good next to the ability input UI?)

**General**
- do I like these games? What other games are like this? --> fighting games. Smash?
- dynamic game needs dynamic changes
    * distance - NUMBER TICKING GOING UP AND DOWN WITH COLOR.
    * stamina/energy/resource - NUMBER. TICKS UP/DOWN DEPENDING ON KAIJU
    
**Input**
- Command Input --> Attack
- Backspace input --> delete previous character from cursor position
- **Design Choice** - No "erase-all" input. Consequence to each thing you type 
    * **TRADE-OFF** - reflex challenge barrier. Said didn't want to do that. Whoever types faster dould have the upper hand?
- Complex input loop - Type in move code. Type number corresponding to resource you want to gain/use. Press enter to confirm

**Status Effects**
- Stunned - Can't input next move. CAN TYPE TO QUEUE NEXT MOVE WHEN STUN FADES.

---
---
---
# MVP Scope

## Intro MVP
Booting Kaiju Fight

## Menu MVP
SF - Start fight
Q - Quit

## FIGHT MVP
### One 'map' (just a name. don't worry about having it affect the battle).

### One kaiju. It can fight itself. KAIJU KING
- Atomic Breath - range of 50. 70 energy cost
- Simple Input loop - Type in move code. Press enter to confirm
    * i.e. Pressed 'K'. Pressed enter. Kaiju kicks.

### Kaiju Systems
- HP (visual)
- Energy (visual)
- MVP Ability System Requirements 
    * Energy Cost (fixed for each ability, visual)
    * Effect (damage, movement, regeneration, etc., visual)
    * Range (just let the player miss for now. No visual)
    * Start-up time (all abilities have a fixed start-up time, VISUAL)
    * Active time (all abilities have a fixed, active time of 0 (instant), NO VISUAL)
    * Cooldown time (all abilities have a fixed cooldown time, VISUAL)
    * All Abilities are non-cancelable.
    * Ability commands are cached. Makes users take care when they are inputting commands
        + Could lead to some funny cheese strategies.
    * No "holding abilities. All abilities have instant active time.
        + (i.e. block continues by holding down confirmation key. For MVP, block has fixed active time.)
- Distance system
- No statuses.

## FIGHT UI MVP
Info for each Kaiju
    - HP
    - Energy
    - Ability state (what ability, what stage of ability)
- Kaiju 1 Info
- Kaiju 2 Info
- Distance

## Technical MVP
Executable I can send to someone and they can play.

CMAKE



---
---
---

# SHOULD I SPLIT THIS GAME INTO THREE? 
- Kaiju Fight - single-player 
- Kaiju Fight Online! (DLC for single-player)
- Kaiju Fight - Story

# Beyond MVP
- Statuses
    * Stunned?
    * Burning
    * Behind Wall
    * Environmental (i.e. burrowed, underwater)
- Complex input loop
    * Active time - different abilities have different active time.
    * Holding abilities. i.e. hold enter to hold block.
    * Takes up UI area where attacks would be?
    * Ability Holding System Options
        1.  Holding a key down affects ability state
            - i.e. charge-up, charge-up visual similar to loading bar.
            - i.e. extends ability's active time to a point (i.e. til runs out of energy)
        2. Command line inputs "lock-in" hold amount 
            - Energy decisions "A30" puts 30 energy into atomic blast. Corresponds to
            - More jank (more dark souls-y). More commitment. Less twitchy. 
            - Maybe different for different kaiju. range.
- Narrative Delivery
    * Message Logs - Reveals lore of world, who you are. Hotline Miami, Bioshock (but more intimate), etc.
        + Player presses enter to read next line? Or just next message?
    * Cyber-Ghost - Each time you defeat a kaiju, a ghost from the past appears, says who they were, what happened. A cautionary tale
        1. Could: The ghost of the man who made kaiju fight. Nobel peace prize.
        2. Or: Satirical. Starship troopers, Mickey 17, etc?
        - Visual: Lines that make a face. Like Starfox Andross but just outlines. static.
- More Kaijus 
    * Kaiju King - Godzilla stand-in. Atomic breath. Regeneration.
    * Dragon King
    * Jumbo Samurai - small kaiju. Physical attacks. spirit.
    * Shinobi Turtle -
    * Max Energy / No-regen - Kaiju that starts with way more energy than anyone else. No regen. Once it runs out of energy, has to run away and find a safe place to recharge/reincarnate in an egg. Extremely vulnerable in egg form. Can break out of egg form early, but then can't get back in until run out of energy.
    * Kaiju that slows down/speeds up time
        + affects warm-up/active/cooldown time for attacks
        + affects anything with a rate based on time (i.e. regeneration)
    * Can't Move Kaiju (like Biollante)
    * **MOTHER DRAGON**
        + Alt name - **MOTHER'S 7 HEADS**
        + A dragon like king ghidorah. There used to be 7 heads. Now, there are 4 heads and 3 stumps. The 3 look like they were bitten off.
            - Maybe one of the seven heads, is robotic. It is the one that bites off the other heads? This, however, changes the metaphor to be
                more about AI, automation, etc. than the original maternal themes. Does it layer the metaphor or muddy the metaphor? Maybe have
                Mother Dragon come back like mecha-king ghidorah. It has new tricks, but, ultimately, it succumbs to the same faults? hmm. 
        + Ability - Compel {Ability}
            - If the player uses any ability other than the one Mother Dragon says to use,... 
                - the player will lose substantial energy.
                - Mother Dragon will fly into a rage, using multi-head attacks against the player
            - If Mother Dragon is at one head left, they can compel the player to use abilities the player does not have access to. (abilities the player has seen in previous monsters)
        + Ability - lies 
            - Insert letters while you type your command OR insert command text into the queue **TAKING AWAY PLAYER'S AGENCY. THAT'S THE FEELING.**
        + How to solve this monster??
            - Mother Dragon uses too many multi-head attacks in a row (gets too low on energy) --> bites a head off --> restores energy.
            - Repeat ^ until the last head heats itself like an oroboros
        + LINES
            - "I fed you my heads, and now you've come for the rest."
            - "Would you like another head, dear?"
        + THEMES
            - Manipulation and guilt, lost agency due to well-meaning maternal figure 
            - This feels true but limited. Adolescent. There is more to say here. How do we go on with these relationships?
                - I dont' think I can expand on these themes without increasing the player's moveset?? hmmm
    * **Father Phallis** - a normal-sized guy who looks like a penis
        + Maybe use distance mechanic. Theme - can't get too close to him. 
        + Turtling strategy
        + 
    * A Kaiju about AI - inspired by Zelda Williams take: https://www.ign.com/articles/robin-williams-daughter-takes-stand-against-ai-slop-exploiting-her-fathers-image 
- More Kaijus. How many though?
- Unlock kaijus as you beat them?


## Narrative
Lore Ideas
- Kaiju Fight 1994 (or 2004?)
- This used to be a defense simulation for kaiju combat and control. That was long ago. Kaiju are dead? Kaiju destroyed everything?
- You are in the future, where things are good. You are assigned to go through archived programs to see if they are still useful.
- This is a dummy training module that was considered defunct. Let coworkers pass notes / leave logs
- Developers are given requirements for a kaiju control training program. As the project develops, the 
requirements become darker, stranger, and more personal.
    * Since the requirements are from on high, the characters know about them only a little bit before
        the player?
    * Themes
        + personal insecurities manifest in our creative work 
        + 

Character Ideas
- Character A - radicalized. Charlie Kirk-like martyr incident. Fully radicalized. Impacts what they want to do with the program.
    
### Sample Message Log - Girlfriend + GUI
    Message Archive - 10/23/2004
    ----------------------------
    G-man (Gary): Hey Trevor, how's it coming.
    T-bone (Trevor): Shits bad, Gary. She was all cool about the flat-earth shit, then I though we were cool, ya know. If flat earth is no problem then normal stuff is normal... so I mentioned I don't like how her Mom talks to her.
    G-man (Gary): I mean the GUI
    T-bone (Trevor): She says, "That's my Mom. That's my problem."
    G-man (Gary): Yup.
    T-bone (Trevor): She goes and blabs. to. her. MOM.
    G-man (Gary): Yikes.
    T-bone (Trevor): !!!! I gotta come on my hands and knees on glass or some shit...
    G-man (Gary): That's rough buddy.
    T-bone (Trevor): Whatever, Gary.
    G-man (Gary): Did you build an encrypted team chat sub-program to talk about your drama?
                I need the GUI mode.
    T-bone (Trevor): The GUI mode is work in progress.
    G-man (Gary): TREVOR
                    I love you.
                    I need a GUI.
                    What's the command?
    T-bone (Trevor): LadyGodiva
    G-man (Gary): Thank you.
    G-man (Gary): Trevor...
    T-bone (Trevor): I'm an artist in pain, Gary.
    G-man (Gary): Trevor. I'm coming to your desk. We are going to make a GUI.
    T-bone (Trevor): Ok.
    

Character Ideas
- T-bone
    + Gets super depressed because of relationship - not because of breakup! But the dynamic.
    + Doesn't have approval of Mom. 



(The GUI Mode is an option for the player. When you put in the GUI command, it reveals a drawing of his soon-to-be Ex-girlfriend).
 (I don't want them to say what it is because I want the player to want to go put the command in later)

CRIT: This is funny. Man relationships. Ignore problems.
- This reads like it's supposed to be shot. I think the nuance in my head is not coming across in the log...
    * **LATER_ME** 
- uncomfortable with the white/black coding tropes.
    * white person is dependable. Isn't personal. Seems like the boss.
    * black person is lost in their personal issues. Likeable, but not dependable.
        + Helped subvert by having Trev be the one who made the encrypted team chat.
    * **ACTION**: Flesh out Gary and Trevor to subvert the stereotypes into real people.
        + Maybe the game is about their relationship? About making something together, under pressure, and building connection.

---
# Beyond v1.0

## 2 player version?
Players press keys on opposite sides of the keyboard. Minimize sabotaging.
- i.e. '`' and '\'
- "hidden" command to do 2 player version. Players who know can jump right in. 

## Kaiju creature generator

---
# QT Doc Help
Inputs key press state - https://somcosoftware.com/en/blog/how-to-track-keyboard-usage-on-windows-with-qt
    
