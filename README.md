![WithdrawalMenu_Screenshot](https://github.com/user-attachments/assets/75f3889d-4ddd-4972-8ca8-e611d07eca8c)

# **Withdrawal** - Team 27

## 👥 Members
- **Project Manager:** Carter Mauer (`cmauer2`)
- **Communications Lead:** Cliff Ricks (`Cwiffy`)
- **Git Master:** Nguyen Vu (`NguyenVu2005`)
- **Design Lead:** Mason Antee (`mas0nn`)
- **Quality Assurance Tester:** *N/A*

---

## 🎮 About Our Project

**Withdrawal** is a psychological horror game that revolves around the player navigating a single, repeating room. The horror experience is designed to be unsettling and psychological, often subverting expectations and playing mind games. The game is inspired by **Silent Hill: P.T.** and **Twelve Minutes**, combining an eerie atmosphere with challenging puzzles and disturbing, subtle horror elements.

### 🔑 Key Features:

- **First Person Perspective** - Designed to maximize immersion, the   player experiences the game from a first-person perspective, enhancing the tension and fear.
- **Single Repeating Room** - The game takes place in a single, repeating room, where each progression is marked by subtle changes and psychological twists.
- **Psychological Horror & Puzzles** - Players must solve puzzles and uncover clues to advance through the game, each step revealing new layers to the mystery.
- **Save & Load System: Game State** - including loop index, player position, flashlight state, and puzzle progress - is persistently saved and can be resumed. 
- **Flashlight Battery Mechanics** - The flashlight consumes battery power over time, requiring players to manage pickups and recharge strategically. 
- **Pause Menu with Options** - Players can pause the game, adjust audio and graphics settings, save progress, or quit via a fully functional in-game UI.

---

## 🖥️ Platforms Tested
- Windows ✅
- MacOS (Editor only) ⚠️
- Linux (Experimental) ⚠️

---

## 🔗 Important Links
- **Kanban Board:** [Link](https://github.com/orgs/CSC-3380-Spring-2025/projects/7/views/1)
- **Designs:** [Link](https://drive.google.com/drive/folders/1XS3uhJ5pH688Lu_2NxWyl6Dbi5qM_QVZ?usp=sharing)
- **Code Standards:** [Link](https://drive.google.com/file/d/16wx_RB980KPqmxKkaCUhvuwlhS4Pcvp3/view?usp=sharing)

---

## 🛠️ How to Download and Run the Game


### 🔧 Dependencies
1. Unreal Engine 5 (required to open and run project)
- [Download Unreal Engine 5](https://www.unrealengine.com/)
2. Visual Studio (for C++ code if you need to modify or recompile the game)
- [Download Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) (with UE5 plugins)
3. Git (version control to sync the latest game files)
- [Download Git](https://git-scm.com/)

---

### 💻 Setup Instructions
To run the project on the `main` branch and test any changes, follow these steps:

#### 1. Clone the Repository (First-Time Setup)
```bash
git clone https://github.com/CSC-3380-Spring-2025/Team-27.git
cd Team-27
```

#### 2. Pull Latest Changes
```bash
git checkout main
git pull origin main
```

#### 3. Copy the `Content` Folder
*Use the `Content` folder shared to you via email or Google Drive*
- Ensure the `Content` folder is placed directly in the root of the project directory (`Team-27/`).
- The folder **must** be named `Content` and not placed inside another subfolder.

#### 4. Open the Project in Unreal Engine 5
- Open **Unreal Engine 5**.
- Click **Browse...**, select `HorrorGame.uproject`, or double-click it in your project folder.

Alternatively, you can directly double-click open the `HorrorGame.uproject` file in the folder to open it in Editor Mode.

#### 5. Recompile and Reload C++ Code
- In UE5, click **Tools > Refresh Visual Studio Project**, then click **Recompile and Reload**.
- Or, manually compile:
  - Open `HorrorGame.sln` in Visual Studio
  - Click **Build > Build Solution**
  - After building, right-click `HorrorGame.uproject` and choose **Generate Visual Studio project files**

---

### ▶️ Playing the Game
- Click **Play** in the editor.
- Interact: `E`  
- Cycle Items: Mouse Scroll  
- Pause Menu: `ESC`
- Flashlight: `F`
- Crouch: `LEFT CTRL`
- Sprint: `LEFT SHIFT`

*There will be an executable file coming soon.*

---

### 🔄 Committing Changes
```bash
git add .
git commit -m "Describe your changes here"
git push origin main
```

---

## 🧪 Troubleshooting Tips
- **Compile Error?**
	- Regenerate Visual Studio project files  
  	- Ensure you're using **Visual Studio 2022**
- **Project won't open or crashes?**  
  - Delete `Saved`, `Intermediate`, and `Binaries` folders  
  - Rebuild via Visual Studio  
  - Re-generate `.uproject` files

*After the files have been rebuilt and regenerated, open the project and try to run the game through the Unreal Engine Editor.*
