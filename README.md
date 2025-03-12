# [First-Person Horror Game] : [Team 27]
# Members
Project Manager: Carter Mauer (cmauer2)\
Communications Lead: Cliff Ricks (Cwiffy)\
Git Master: Nguyen Vu (NguyenVu2005)\
Design Lead: Mason Antee (mas0nn)\
Quality Assurance Tester: Malachi Fowler (MalachiF18)

# About Our Software

This project is a psychological horror game that revolves around the player navigating a single, repeating room. The horror experience is designed to be unsettling and psychological, often subverting expectations and playing mind games. The game is inspired by Silent Hill: P.T. and Twelve Minutes, combining an eerie atmosphere with challenging puzzles and disturbing, subtle horror elements.

Key Features:

- First Person Perspective: Designed to maximize immersion, the   player experiences the game from a first-person perspective, enhancing the tension and fear.
- Single Repeating Room: The game takes place in a single, repeating room, where each progression is marked by subtle changes and psychological twists.
- Psychological Horror & Puzzles: Players must solve puzzles and uncover clues to advance through the game, each step revealing new layers to the mystery.

The project is built as a client-side desktop application using Unreal Engine 5.

## Platforms Tested on
- Windows
- MacOS
- Linux

# Important Links
Kanban Board: [Kanban Board](https://github.com/orgs/CSC-3380-Spring-2025/projects/7/views/1) \
Designs: [link]\
Styles Guide(s): [link]

# How to Download and Run the Game

## Dependencies
1. Unreal Engine 5 (required to open and run project)
- [Download Unreal Engine 5](https://www.unrealengine.com/)
2. Visual Studio (for C++ code if you need to modify or recompile the game)
- [Download Visual Studio](https://code.visualstudio.com/)
3. Git (version control to sync the latest game files)
- [Download Git](https://git-scm.com/)

Make sure to use the free version of Visual Studio and download the required Unreal Engine plugin/extensions for development.

## Commands
To run the project on the `main` branch and test any changes, follow these steps:

### 1. Clone the Repository (First-Time Setup)
If you haven't already cloned the repository, use the following command:
```sh
git clone https://github.com/CSC-3380-Spring-2025/Team-27.git
cd Team-27
```

### 2. Pull Latest Changes
Before launching the project, ensure you have the latest updates from the `main` branch:
```sh
git checkout main
git pull origin main
```

### 3. Open the Project in Unreal Engine 5
1. Launch **Unreal Engine 5**.
2. Click **Browse...** in the Unreal Engine launcher.
3. Navigate to the folder where you cloned the repository and select the `HorrorGame.uproject` file.

Alternatively, you can directly double-click open the `HorrorGame.uproject` file in the folder to open it in Editor Mode

### 4. Recompile and Reload C++ Code
If the game doesn't run immediately or you see errors, you may need to recompile the C++ code: 
1. In the Unreal Engine 5 editor, make sure to click the "Recompiles and Reloads C++ code for game systems on the fly" button, located next to the revision control at the bottom of the screen.
2. If this option does not work, you may need to compile the C++ code manually in **Visual Studio**. 
	- Open the project in **Visual Studio** by locating the `HorrorGame.uproject` file and selecting "Generate Visual Studio project files."
	- Go into Visual Studio and select **Build -> Build Solution**

Once built and compiled, you should be able to run the game.

### 5. Playing the Game
To start the game in **Unreal Engine**:
1. Click **Play** in the Unreal Engine editor.
2. You will be able to test the game directly from the editor.

(There will be an executable file coming soon)

### 6. Committing and Pushing Changes
After testing, if you've made changes that need to be shared:
```sh
git add .
git commit -m "Describe your changes here"
git push origin main
```

