Aim Trainer Pro
Un trainer d'aim professionnel inspiré d'Aim Lab avec un thème spatial époustouflant ! Améliorez votre précision et votre réactivité dans un environnement immersif.

🎯 Fonctionnalités
Gameplay style Aim Lab :

Entraînement de précision : Cliquez sur les cibles colorées qui apparaissent
Cibles dynamiques : Tailles variables et couleurs procédurales
Timer de 45 secondes : Sessions d'entraînement courtes et intenses
Système de score avancé : Points basés sur la taille des cibles (plus petite = plus de points)
Interface complète :

Menu principal navigable avec les flèches
Compte à rebours de 5 secondes avant chaque partie
Statistiques détaillées : Score, précision, temps restant
Scoreboard persistant : Sauvegarde automatique de vos meilleurs scores
Ambiance sonore :

Musique de fond Valorant (optionnelle)
Contrôle audio : On/Off depuis le menu
Effets visuels spatiaux : Étoiles scintillantes, planète en rotation, explosions de particules
🎮 Contrôles
Dans le menu :

Flèches Haut/Bas : Navigator dans le menu
Entrée/Espace : Sélectionner une option
En jeu :

Clic gauche : Tirer sur les cibles
Échap : Retour au menu (optionnel)
Écran de fin :

Espace : Retourner au menu principal
🎯 Comment jouer
Menu Principal : Naviguez avec les flèches et sélectionnez "PLAY"
Compte à rebours : Préparez-vous pendant les 5 secondes
Session d'entraînement : 45 secondes pour toucher un maximum de cibles
Cibles variables : Plus la cible est petite, plus elle rapporte de points
Résultats : Consultez votre score et votre précision
Scoreboard : Vos meilleurs scores sont automatiquement sauvegardés
Prerequisites
Before building and running the game, you need to have CSFML installed on your system.

System Requirements
GCC compiler with C99 support
CSFML library (graphics, audio, system modules)
Make utility
Linux/macOS/Windows with MinGW
Installation
1. Clone or Download the Project
bash
# If using git
git clone <repository-url>
cd duck_hunt

# Or download and extract the source files
2. Install CSFML Dependencies
The project includes an automatic dependency installer for common package managers:

bash
make install_deps
This will automatically detect your package manager and install CSFML. Supported systems:

Ubuntu/Debian: sudo apt-get install libcsfml-dev
Arch Linux: sudo pacman -S csfml
macOS: brew install csfml
Fedora: sudo dnf install CSFML-devel
Manual Installation
If the automatic installer doesn't work for your system:

Ubuntu/Debian:
bash
sudo apt-get update
sudo apt-get install libcsfml-dev
Arch Linux:
bash
sudo pacman -S csfml
macOS (with Homebrew):
bash
brew install csfml
Fedora/CentOS/RHEL:
bash
sudo dnf install CSFML-devel
# or for older versions:
sudo yum install CSFML-devel
Windows:
Download CSFML from https://www.sfml-dev.org/download/csfml/
Extract to a directory (e.g., C:\CSFML)
Add the bin directory to your PATH
Modify the Makefile to point to your CSFML installation
Building the Game
Quick Build and Run
bash
# Build and run in one command
make run
Step-by-Step Build
bash
# Clean any previous builds
make clean

# Build the project
make

# Run the game
./duck_hunt
Debug Build
bash
# Build with debug symbols
make debug
Project Structure
duck_hunt/
├── main.c          # Entry point and main game loop
├── game.c          # Game initialization, update, and rendering
├── duck.c          # Duck behavior and management
├── utils.c         # Utility functions
├── duck_hunt.h     # Header file with all declarations
├── Makefile        # Build configuration
└── README.md       # This file
🎯 Gameplay Instructions
Starting the Game: Run ./duck_hunt to enter the space battlefield
Firing Plasma: Click anywhere to fire your plasma weapon
Scoring: Each alien ship destroyed gives you 100 points
Plasma System: You have 3 plasma shots per round
Reloading: After using all 3 shots, plasma automatically refills
Alien Ship Behavior:
Ships spawn every 2 seconds from random locations
They move in unpredictable flight patterns
They bounce off screen edges like real spacecraft
Each ship has unique colors and rotation patterns
Spectacular particle explosions when destroyed
Code Organization
The project follows strict coding standards:

Maximum 5 functions per file
Maximum 35 lines per function
Modular design with separated concerns
Clean memory management with proper cleanup
File Responsibilities
main.c: Program entry point and event handling
game.c: Core game logic, space rendering, and particle systems
duck.c: Alien ship spawning, movement, and visual effects (renamed but functionality expanded)
utils.c: Space texture generation, explosions, and helper functions
duck_hunt.h: All structure definitions and function prototypes
Troubleshooting
Common Issues
"Command not found: gcc"
bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install
"CSFML not found"
Run make install_deps first
Check if CSFML is properly installed
Verify library paths in your system
"Permission denied"
bash
chmod +x duck_hunt
Linker errors
Ensure all CSFML libraries are installed
Check library paths with ldconfig -p | grep csfml
Debug Information
To see detailed compilation information:

bash
make clean && make debug
Performance Notes
The game runs at smooth 60 FPS with advanced visual effects
200 animated stars with individual twinkling patterns
Real-time particle system with up to 50 explosion particles
Optimized texture generation and memory management
No external asset files required (all graphics generated programmatically)
Visual Features
Deep Space Background: Rich dark blue space atmosphere
Dynamic Starfield: 200 stars with individual brightness and twinkling speeds
Alien Ships: Procedurally colored ships with rotation and scaling animations
Particle Explosions: Multi-colored explosion effects with physics
Planetary Element: Rotating orange planet for atmospheric depth
Colorful HUD: Cyan score, orange plasma counter, purple title text
License
This project is created for educational purposes. Feel free to modify and distribute.

Contributing
Feel free to submit issues and pull requests. Please maintain the coding standards:

Maximum 5 functions per file
Maximum 35 lines per function
Follow existing code style
Test thoroughly before submitting
Future Enhancements
Possible cosmic improvements:

Space ambient sounds and laser sound effects
Epic space battle music
Different alien ship types with unique behaviors
Power-ups and special weapons
Cosmic high score system with star rankings
Asteroid field obstacles
Multiple star system backgrounds
Boss battles with massive alien motherships
