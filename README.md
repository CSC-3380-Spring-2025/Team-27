
![WithdrawalMenu_Screenshot](https://github.com/user-attachments/assets/75f3889d-4ddd-4972-8ca8-e611d07eca8c)

# Withdrawal : Team 27
# Members
Project Manager: Carter Mauer (cmauer2)\
Communications Lead: Cliff Ricks (Cwiffy)\
Git Master: Nguyen Vu (NguyenVu2005)\
Design Lead: Mason Antee (mas0nn)\
Quality Assurance Tester: N/A

# About Our Project

This project is a psychological horror game that revolves around the player navigating a single, repeating room. The horror experience is designed to be unsettling and psychological, often subverting expectations and playing mind games. The game is inspired by Silent Hill: P.T. and Twelve Minutes, combining an eerie atmosphere with challenging puzzles and disturbing, subtle horror elements.

Key Features:

- First Person Perspective: Designed to maximize immersion, the   player experiences the game from a first-person perspective, enhancing the tension and fear.
- Single Repeating Room: The game takes place in a single, repeating room, where each progression is marked by subtle changes and psychological twists.
- Psychological Horror & Puzzles: Players must solve puzzles and uncover clues to advance through the game, each step revealing new layers to the mystery.
- Save & Load System: Game State - including loop index, player position, flashlight state, and puzzle progress - is persistently saved and can be resumed. 
- Flashlight Battery Mechanics: The flashlight consumes battery power over time, requiring players to manage pickups and recharge strategically. 
- Pause Menu with Options: Players can pause the game, adjust audio and graphics settings, save progress, or quit via a fully functional in-game UI.

The project is built as a client-side desktop application using Unreal Engine 5.

## Platforms Tested on
- Windows
- MacOS (Editor only)
- Linux (Experimental)

# Important Links
Kanban Board: [Link](https://github.com/orgs/CSC-3380-Spring-2025/projects/7/views/1) \
Designs: [Link](https://drive.google.com/drive/folders/1XS3uhJ5pH688Lu_2NxWyl6Dbi5qM_QVZ?usp=sharing) \
Styles Guide(s): [Link](https://github.com/user-attachments/files/20096032/Git.Master.-.Code.Standards.pdf)


# How to Download and Run the Game

## Dependencies
1. Unreal Engine 5 (required to open and run project)
- [Download Unreal Engine 5](https://www.unrealengine.com/)
2. Visual Studio (for C++ code if you need to modify or recompile the game)
- [Download Visual Studio 2022](https://code.visualstudio.com/)
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

### 3. Copy the Content Folder
Before opening the project in Unreal Engine, make sure the `Content` folder is in your cloned project directory:
Simply copy and paste the entire `Content` folder into the root directory of the project (`Team-27/`).

- If you're downloading it from a zip, make sure the Content folder isn't inside another folder and it's all by itself, otherwise the game won't load properly. Also, make sure the folder is named `Content` and not something else.

### 4. Open the Project in Unreal Engine 5
1. Launch **Unreal Engine 5**.
2. Click **Browse...** in the Unreal Engine launcher.
3. Navigate to the folder where you cloned the repository and select the `HorrorGame.uproject` file.

Alternatively, you can directly double-click open the `HorrorGame.uproject` file in the folder to open it in Editor Mode.

### 5. Recompile and Reload C++ Code
If the game doesn't run immediately or you see errors, you may need to recompile the C++ code: 
1. In the Unreal Engine 5 editor, make sure to click **Tools** -> **Refresh Visual Studio Project** then the **Recompiles and Reloads C++ code for game systems on the fly** button, located next to the revision control at the bottom of the screen.
2. If this option does not work, you may need to compile the C++ code manually in **Visual Studio**. 
	- Open the project in **Visual Studio** by locating the `HorrorGame.sln` and selecting **Build -> Build Solution**
	- After that, close out of Visual Studio, right click the `HorrorGame.uproject` file, and select **Generate Visual Studio project files**. 

Once built and compiled, you should be able to run the game.

### 6. Playing the Game
To start the game in **Unreal Engine**:
1. Click **Play** in the Unreal Engine editor.
2. You will be able to test the game directly from the editor.
3. Interact with objects using the **E** key.
4. Use scroll wheel to switch inventory items.
5. Use **ESC** to open the pause menu.

(There will be an executable file coming soon)

### 7. Committing and Pushing Changes
After testing, if you've made changes that need to be shared:
```sh
git add .
git commit -m "Describe your changes here"
git push origin main
```

## Troubleshooting Tips
- **Compile Error?**
	- Regenerate VS project files
	- Double check you're using **VS 2022** with UE5 toolchain.
- **Still Doesn't Run?**
	- Go into you're cloned repository folder
	- Delete these folders: `Saved`, `Intermediate`, `Binaries`.
	- Once these are deleted, open Visual Studio, click the dropdown **Build -> Build/Rebuild Solution**
	- Close out of Visual Studio, then right click **HorrorGame.uproject -> Generate Visual Studio project files**. 

After the files have been rebuilt and regenerated, open the project and try to run the game through the Unreal Engine Editor.
