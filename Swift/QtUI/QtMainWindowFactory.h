#ifndef SWIFT_QtMainWindowFactory_H
#define SWIFT_QtMainWindowFactory_H

#include "Swift/Controllers/MainWindowFactory.h"

namespace Swift {
	class QtTreeWidgetFactory;
	class QtMainWindowFactory : public MainWindowFactory{
		public:
			QtMainWindowFactory(QtTreeWidgetFactory *treeWidgetFactory);
			MainWindow* createMainWindow();
		private:
			QtTreeWidgetFactory *treeWidgetFactory_;
	};
}

#endif
