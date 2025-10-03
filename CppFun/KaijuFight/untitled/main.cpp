#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // 

    //////////////////////////////////////////////
    /// APP FLOW
    /// //////////////////////////////////////////
    /// setup MVP
    ///     window MVP
    ///     Load data (kaiju data) MVP
    ///     Load
    ///     Intro sequence (You are logging into an old console) *P3*
    ///          Booting Kaiju Fight DOS style load screen. '*' filling across screen as it loads
    ///

    ///
    /// Main Menu loop
        /// Fight MVP
        /// View Kaiju Data
    /// Fight
    ///     Fight UI
    ///     Pre-battle
    ///     Fight

    w.show();
    return a.exec();
}



/// real-time combat puzzle --
/// Number + Letter --> Attack
/// do I like these games? What other games are like this?
///     fighting games. Smash?
/// dynamic game needs dynamic changes
///     distance - NUMBER TICKING GOING UP AND DOWN WITH COLOR.
///     stamina/energy/resource - NUMBER. TICKS UP/DOWN DEPENDING ON KAIJU
/// Stunned
///     Can't input next move. CAN TYPE TO QUEUE NEXT MOVE WHEN STUN FADES.
///
/// Complex input loop - Type in move code. Type number corresponding to resource you want to gain/use. Press enter to confirm
///
/// DO'S / DON'TS
///     NO COMPLEX COMBOS TO DO A MOVE. EMPHASIS ON PUZZLE STRATEGY THAN INPUT CHALLENGE
///     All monsters have the same max energy. Easier to balance. Easier for players to understand
///     SCREW BALANCE
///
/// Design ?'s
///     Game Speed is directly related to kaiju speed. How slow? How fast?
///     Key placement affects feeling for each kaiju
///         Playing a kaiju with all inputs in one place vs. across the keyboard - Feel like a spider/master controller vs. a league character)
///     Fight Length 2-5 minutes? Lumbering kaiju. Should feel like controlling massive beasts where the fight sways back and forth.
///         But will that be too slow? Will that be fun? The fight needs to be able to turn quickly to have uncertainty to be fun.
///
/// Intro MVP
///     Booting Training Module
///     Kaiju Fight
///
/// Menu MVP
///     Start fight
///     Quit
///
/// Battle MVP
///     One 'map' (just a name. don't worry about having it affect the battle).
///     One kaiju. It can fight itself. KAIJU KING
///         - Atomic Breath - range of 50. 70 energy cost
///         -
///     Simple Input loop - Type in move code. Press enter to confirm
///         i.e. Pressed 'K'. Pressed enter. Kaiju kicks.
///     Kaiju Systems
///         HP
///         Energy
///         Ability System
///             Energy Cost (fixed for each ability)
///             Effect (damage, movement, regeneration, etc.)
///             Range (just let the player miss for now. No visual)
///             Start-up time (all abilities have a fixed start-up time, VISUAL)
///             Active time (all abilities have a fixed, active time of 0 (instant), NO VISUAL)
///             Cooldown time (all abilities have a fixed cooldown time, VISUAL)
///             All Abilities are non-cancelable.
///             No "holding abilities. All abilities have instant active time.
///                  (i.e. block continues by holding down confirmation key. For MVP, block has fixed active time.)
///         Distance system
///         No statuses.
///
/// Technical MVP
///     Executable I can send to someone and they can play.
///
///
/// Fight UI MVP
///     Kaiju 1 Info
///         HP
///         Energy
///         Ability
///     Kaiju 2 Info
///     Distance
///
/// Beyond MVP
///     (fixed for each ability, VISUAL? Or just let the player miss? An indicator when distance is good next to the ability input UI?)
///
///     Statuses
///         Stunned?
///         Burning
///         Behind Wall
///
///     Complex input loop
///         Energy decisions "A30" puts 30 energy into atomic blast. Corresponds to range.
///
///      Active time - different abilities have different active time.
///
///      Holding abilities. i.e. hold enter to hold block.
///         Takes up UI area where attacks would be?
///         Holding a key down affects ability state
///              i.e. charge-up, charge-up visual similar to loading bar.
///              i.e. extends ability's active time to a point (i.e. til runs out of energy)
///
///     More Kaijus. How many though?
///
///     Environmental effects/statuses/impact (i.e. burrowed, underwater)
///
///     Unlock kaijus as you beat them?
///
///     Conversation mini-game outside fight. Reveals lore of world, who you are. Similar to Hotline Miami.
///         This used to be a defense simulation for kaiju combat and control. That was long ago. Kaiju are dead? Kaiju destroyed everything?
///         The ghost of the man who made kaiju fight. Nobel peace prize.
///         You are in the future, where things are good. You are assigned to go through archived programs to see if they are still useful.
///             Each time you defeat a kaiju, a ghost from the past appears, says who they were, what happened. A cautionary tale
///             Lines that make a face. Like Starfox Andross but just outlines. static.
///         Character A - radicalized. Charlie Kirk-like martyr incident. Fully radicalized. Impacts what they want to do with the program.
///         Player presses enter to read next line? Or just next message?
///     Kaiju Fight 2084
///     This is a dummy training module that was considered defunct. Let coworkers pass notes / leave logs
///         dude (Trevor, username is T-bone) talking about relationship drama.
///         CRIT: This is funny. Man relationships. Ignore problems.
///             uncomfortable with the white/black coding tropes.
///                 - white person is dependable. Isn't personal. Seems like the boss.
///                 - black person is lost in their personal issues. Likeable, but not dependable.
///             Does this work if I swap ethnicity, genders, sexual orientations?
///                 - Gary is a lesbian. Doesn't chang the race dynamic.
///                 - Trevor is
///
///         G-man (Gary): Hey Trevor, how's it coming.
///         T-bone (Trevor): Shits bad, Gary. She was all cool about the flat-earth shit, then I though we were cool, ya know. If flat earth is
///             no problem then normal stuff is normal... so I mentioned I don't like how her Mom talks to my girl.
///         G-man (Gary): I mean the GUI
///         T-bone (Trevor): She says, "That's my Mom. That's my problem."
///         G-man (Gary): Yup.
///         T-bone (Trevor): She goes and blabs. to. her. MOM.
///         G-man (Gary): Yikes.
///         T-bone (Trevor): !!!! I gotta come on my hands and knees on glass or some shit...
///         G-man (Gary): That's rough buddy.
///         T-bone (Trevor): Whatever, Gary.
///         G-man (Gary): I didn't make an encrypted chatroom to talk about relationship issues. I need the GUI mode.
///         T-bone (Trevor): It's work in progress.
///         G-man (Gary): TREVOR
///                         I love you.
///                         I need a GUI.
///                         What's the command?
///         T-bone (Trevor): LadyGodiva
///         G-man (Gary): Trevor...
///         T-bone (Trevor): I'm an artist in pain, Gary.
///                         (I don't want them to say what it is because I want the player to want to go put the command in later)
///         G-man (Gary): Trevor. I'm coming to your desk. We are going to make a GUI.
///         T-bone (Trevor): Ok.
///         (The GUI Mode is an option for the player. When you put in the GUI command, it reveals a drawing of his soon-to-be Ex-girlfriend).
///
///
///
///
/// Beyond v1.0
///     2 player version?
///         Players press keys on opposite sides of the keyboard. Minimize sabotaging.
///             i.e. '`' and '\'
///         "hidden" file or command to do 2 player version
///
///     Kaiju creature generator
///
///
///
/// KAIJU IDEAS
///     Kaiju King - Godzilla stand-in. Atomic breath. Regeneration.
///     Dragon King
///     Jumbo Samurai - small kaiju. Physical attacks. spirit.
///     Shinobi Turtle -
///     Max Energy / No-regen - Kaiju that starts with way more energy than anyone else. No regen. Once it runs out of energy, has to run away and
///         find a safe place to recharge/reincarnate in an egg. Extremely vulnerable in egg form. Can break out of egg form early, but then can't
///         get back in until run out of energy.
///     Kaiju that slows down/speeds up time
///
///
/// Turn-based - nah. dunno how to make that in a way that isn't pokemon, and I don't want to make pokemon. Kinda sucks.
///      puzzle with chance element (elements + dice)
///
///
/// tech help
///     key press state - https://somcosoftware.com/en/blog/how-to-track-keyboard-usage-on-windows-with-qt



