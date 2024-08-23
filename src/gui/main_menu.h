#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>


struct Menu_Main_File { // OK

	QMenu* menuFile;

	Menu_Main_File(QMenuBar* menubar) {

		menuFile = new QMenu(menubar);
		menuFile->setObjectName("menuFile");

	}

};

struct Menu_Main_Edit { // OK

	QMenu* menuEdit;

	Menu_Main_Edit(QMenuBar* menubar) {

		menuEdit = new QMenu(menubar);
		menuEdit->setObjectName("menuEdit");

	}
};

struct Menu_Main_Game { // OK

	QMenu* menuGame;

	QAction* actionNewGame;
	QMenu* menuHistory;
	QAction* actionStopGame;

	// hline

	QMenu* menuSelect_Piece;

	QMenu* menuMove;
	QAction* actionNORTH;
	QAction* actionEAST;
	QAction* actionSOUTH;
	QAction* actionWEST;

	//undo: ->
	QAction* actionMoveBack; // rename: actionUndo

	//hline

	QAction* actionStart_Solver;
	QAction* actionStop_Solver;

	QMenu* menuPlaySolver;
	QAction* actionForward;
	QAction* actionBack;

	QAction* actionSolver_Configuration;




	// solver configuration


	Menu_Main_Game(QMenuBar* menubar) {

		menuGame = new QMenu(menubar);
		menuGame->setObjectName("menuGame");

		menubar->addAction(menuGame->menuAction());

		actionNewGame = new QAction(menuGame);//was parent mainWindow
		actionNewGame->setObjectName("actionNewGame");
		actionNewGame->setEnabled(false);

		menuHistory = new QMenu(menuGame);
		menuHistory->setObjectName("menuHistory");

		actionStopGame = new QAction(menuGame); //was parent mainWindow
		actionStopGame->setObjectName("actionStopGame");
		actionStopGame->setEnabled(false);

		menuSelect_Piece = new QMenu(menuGame);
		menuSelect_Piece->setObjectName("menuSelect_Piece");

		menuMove = new QMenu(menuGame);
		menuMove->setObjectName("menuMove");

		{
			actionNORTH = new QAction(menuMove);
			actionNORTH->setObjectName("actionNORTH");
			actionNORTH->setEnabled(true);

			actionEAST = new QAction(menuMove);
			actionEAST->setObjectName("actionEAST");
			actionEAST->setEnabled(true);

			actionSOUTH = new QAction(menuMove);
			actionSOUTH->setObjectName("actionSOUTH");
			actionSOUTH->setEnabled(true);

			actionWEST = new QAction(menuMove);
			actionWEST->setObjectName("actionWEST");
			actionWEST->setEnabled(true);

			menuMove->addAction(actionNORTH);
			menuMove->addAction(actionEAST);
			menuMove->addAction(actionSOUTH);
			menuMove->addAction(actionWEST);

		}


		actionMoveBack = new QAction(menuGame); //was parent mainWindow
		actionMoveBack->setObjectName("actionMoveBack");
		actionMoveBack->setEnabled(false);


		actionStart_Solver = new QAction(menuGame);
		actionStart_Solver->setObjectName("actionStart_Solver");
		actionStart_Solver->setEnabled(false);

		actionStop_Solver = new QAction(menuGame);
		actionStop_Solver->setObjectName("actionStop_Solver");
		actionStop_Solver->setEnabled(false);

		menuPlaySolver = new QMenu(menuGame);
		menuPlaySolver->setObjectName("menuPlaySolver");

		{
			actionForward = new QAction(menuPlaySolver);
			actionForward->setObjectName("actionForward");
			actionForward->setEnabled(true);

			actionBack = new QAction(menuPlaySolver);
			actionBack->setObjectName("actionBack");
			actionBack->setEnabled(true);

			menuPlaySolver->addAction(actionForward);
			menuPlaySolver->addAction(actionBack);
		}

		actionSolver_Configuration = new QAction(menuGame);
		actionSolver_Configuration->setObjectName("actionSolver_Configuration");
		actionSolver_Configuration->setEnabled(false);


		menuGame->addAction(actionNewGame);
		menuGame->addAction(menuHistory->menuAction());
		menuGame->addAction(actionStopGame);
		menuGame->addSeparator();
		menuGame->addAction(menuSelect_Piece->menuAction());
		menuGame->addAction(menuMove->menuAction());
		menuGame->addAction(actionMoveBack);
		menuGame->addSeparator();
		menuGame->addAction(actionStart_Solver);
		menuGame->addAction(actionStop_Solver);
		menuGame->addAction(menuPlaySolver->menuAction());
		menuGame->addAction(actionSolver_Configuration);
	}

};

struct Menu_Main_Developer { // OK

	QMenu* menuDeveloper;

	QAction* actionHighlightGeneratedTargetCells;
	QAction* actionEnableAllMenuBarItems;
	QAction* action22ReferenceGame;


	Menu_Main_Developer(QMenuBar* menubar) {

		menuDeveloper = new QMenu(menubar);
		menuDeveloper->setObjectName("menuDeveloper");

		actionHighlightGeneratedTargetCells = new QAction(menuDeveloper);//MainWindow
		actionHighlightGeneratedTargetCells->setObjectName("actionHighlightGeneratedTargetCells");
		actionHighlightGeneratedTargetCells->setEnabled(true);

		actionEnableAllMenuBarItems = new QAction(menuDeveloper);
		actionEnableAllMenuBarItems->setObjectName("actionEnableAllMenuBarItems");
		actionEnableAllMenuBarItems->setEnabled(true);

		action22ReferenceGame = new QAction(menuDeveloper);
		action22ReferenceGame->setObjectName("action22ReferenceGame");
		action22ReferenceGame->setEnabled(true);

		//menuDeveloper->addAction(actionAbout);
		menuDeveloper->addAction(actionHighlightGeneratedTargetCells);
		menuDeveloper->addAction(actionEnableAllMenuBarItems);
		menuDeveloper->addAction(action22ReferenceGame);


	}
};

struct Menu_Main_View { // OK

	QMenu* menuView;

	QMenu* menuPieces;

	Menu_Main_View(QMenuBar* menubar) {
		
		menuView = new QMenu(menubar);
		menuView->setObjectName("menuView");

		menuPieces = new QMenu(menuView);
		menuPieces->setObjectName("menuPieces");

		menuView->addAction(menuPieces->menuAction());

	}

};

struct Menu_Main_Help { // OK

	QMenu* menuHelp;

	QAction* actionAbout;
	QAction* actionLicense_Information;


	Menu_Main_Help(QMenuBar* menubar) {
		menuHelp = new QMenu(menubar);
		menuHelp->setObjectName("menuHelp");

		actionAbout = new QAction(menuHelp); //mainWindow
		actionAbout->setObjectName("actionAbout");
		actionAbout->setEnabled(true);

		actionLicense_Information = new QAction(menuHelp);
		actionLicense_Information->setObjectName("actionLicense_Information");
		actionLicense_Information->setEnabled(true);

		menuHelp->addAction(actionAbout);
		menuHelp->addAction(actionLicense_Information);

	}
};







struct Menu_Main_Root { // OK


	Menu_Main_File file;
	Menu_Main_Edit edit;
	Menu_Main_Game game;
	Menu_Main_Developer developer;
	Menu_Main_View view;
	Menu_Main_Help help;

	Menu_Main_Root(QMenuBar* menubar) :file(menubar), edit(menubar), game(menubar), developer(menubar), view(menubar), help(menubar) {

		menubar->addAction(file.menuFile->menuAction());
		menubar->addAction(edit.menuEdit->menuAction());
		menubar->addAction(game.menuGame->menuAction());
		menubar->addAction(developer.menuDeveloper->menuAction());
		menubar->addAction(view.menuView->menuAction());
		menubar->addAction(help.menuHelp->menuAction());

	}

};


struct MenuBar_Main { // OK

	QMenuBar* menubar;

	Menu_Main_Root* rootMenu;

	MenuBar_Main(QMainWindow* MainWindow) {

		menubar = new QMenuBar(MainWindow);
		menubar->setObjectName("menubar");
		menubar->setGeometry(QRect(0, 0, 1097, 22));

		rootMenu = new Menu_Main_Root(menubar);
		// ### need destructor for this!
	}
};