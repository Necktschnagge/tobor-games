#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QMainWindow>


// Note that by auto-generated code, all actions are created with mainWindow as their parent

struct Menu_Main_File {

	QMenu* menuFile;

	Menu_Main_File(QMenuBar* menubar) {
		menuFile = new QMenu(menubar);
		menuFile->setObjectName("menuFile");
	}

};

struct Menu_Main_Edit {

	QMenu* menuEdit;

	Menu_Main_Edit(QMenuBar* menubar) {
		menuEdit = new QMenu(menubar);
		menuEdit->setObjectName("menuEdit");
	}
};

struct Menu_Main_Game {

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

	QAction* actionUndo;

	//hline

	QAction* actionStart_Solver;
	QAction* actionStop_Solver;

	QMenu* menuPlaySolver;
	QAction* actionForward;
	QAction* actionBack;

	QAction* actionSolver_Configuration;


	Menu_Main_Game(QMenuBar* menubar) {

		menuGame = new QMenu(menubar);
		menuGame->setObjectName("menuGame");

		menubar->addAction(menuGame->menuAction());

		actionNewGame = new QAction(menuGame);
		actionNewGame->setObjectName("actionNewGame");
		actionNewGame->setEnabled(false);

		menuHistory = new QMenu(menuGame);
		menuHistory->setObjectName("menuHistory");

		actionStopGame = new QAction(menuGame);
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

		actionUndo = new QAction(menuGame);
		actionUndo->setObjectName("actionUndo");
		actionUndo->setEnabled(false);

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
		menuGame->addAction(actionUndo);
		menuGame->addSeparator();
		menuGame->addAction(actionStart_Solver);
		menuGame->addAction(actionStop_Solver);
		menuGame->addAction(menuPlaySolver->menuAction());
		menuGame->addAction(actionSolver_Configuration);
	}

};

struct Menu_Main_Developer {

	QMenu* menuDeveloper;

	QAction* actionHighlightGeneratedTargetCells;
	QAction* actionEnableAllMenuBarItems;
	QAction* action22ReferenceGame;
	QAction* actionViewSVGInput;

	Menu_Main_Developer(QMenuBar* menubar) {

		menuDeveloper = new QMenu(menubar);
		menuDeveloper->setObjectName("menuDeveloper");

		actionHighlightGeneratedTargetCells = new QAction(menuDeveloper);
		actionHighlightGeneratedTargetCells->setObjectName("actionHighlightGeneratedTargetCells");
		actionHighlightGeneratedTargetCells->setEnabled(true);

		actionEnableAllMenuBarItems = new QAction(menuDeveloper);
		actionEnableAllMenuBarItems->setObjectName("actionEnableAllMenuBarItems");
		actionEnableAllMenuBarItems->setEnabled(true);

		action22ReferenceGame = new QAction(menuDeveloper);
		action22ReferenceGame->setObjectName("action22ReferenceGame");
		action22ReferenceGame->setEnabled(true);

		actionViewSVGInput = new QAction(menuDeveloper);
		actionViewSVGInput->setObjectName("actionViewSVGInput");
		actionViewSVGInput->setEnabled(true);

		menuDeveloper->addAction(actionHighlightGeneratedTargetCells);
		menuDeveloper->addAction(actionEnableAllMenuBarItems);
		menuDeveloper->addAction(action22ReferenceGame);
		menuDeveloper->addAction(actionViewSVGInput);
	}
};

struct Menu_Main_View {

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

struct Menu_Main_Help {

	QMenu* menuHelp;

	QAction* actionAbout;
	QAction* actionLicense_Information;


	Menu_Main_Help(QMenuBar* menubar) {
		menuHelp = new QMenu(menubar);
		menuHelp->setObjectName("menuHelp");

		actionAbout = new QAction(menuHelp);
		actionAbout->setObjectName("actionAbout");
		actionAbout->setEnabled(true);

		actionLicense_Information = new QAction(menuHelp);
		actionLicense_Information->setObjectName("actionLicense_Information");
		actionLicense_Information->setEnabled(true);

		menuHelp->addAction(actionAbout);
		menuHelp->addAction(actionLicense_Information);
	}
};

struct Menu_Main_Root {


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

struct MenuBar_Main {

	QMenuBar* menubar;

	Menu_Main_Root* rootMenu;

	MenuBar_Main(QMainWindow* MainWindow) {
		menubar = new QMenuBar(MainWindow);
		menubar->setObjectName("menubar");
		menubar->setGeometry(QRect(0, 0, 1097, 22));

		rootMenu = new Menu_Main_Root(menubar);
	}

	~MenuBar_Main() {
		delete rootMenu;
		// we might think about using smart pointer to get rid of the delete, or let these classes become QObjects so that we can rely on parenting
	}
};
