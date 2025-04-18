/*
 * Copyright (C) 2023 Emweb bv, Herent, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include <boost/test/unit_test.hpp>

#include <web/DomElement.h>
#include <web/WebSession.h>

#include <Wt/WApplication.h>
#include <Wt/WBootstrap2Theme.h>
#include <Wt/WBootstrap3Theme.h>
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WStringListModel.h>
#include <Wt/WTableView.h>
#include <Wt/WTabWidget.h>
#include <Wt/WText.h>
#include <Wt/WToolBar.h>
#include <Wt/WTree.h>
#include <Wt/WTreeTable.h>
#include <Wt/WTreeTableNode.h>
#include <Wt/WTreeView.h>
#include <Wt/Test/WTestEnvironment.h>

// Note: In this file tests are repeated. This is done to test multiple themes.
// If no theme is specified, the default of WCssTheme is used.
// Then, a Bootstrap theme is selected each time in alternating fashion.
// For all Bootstrap themes the disabled class name is always the same, so their
// outcomes ought to be identical.

BOOST_AUTO_TEST_CASE( WCompositeWidget_WMenu_setDisabled )
{
  // Ensures that when a WMenu is disabled its menu items are visually disabled as well (but not in code).
  // If the WMenuItem alone is disabled, the WMenu parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto menu = std::make_unique<Wt::WMenu>();
  auto menuItem = menu->addItem("Item 1");
  auto menuPtr = menu.get();
  app.root()->addWidget(std::move(menu));

  // Simulate UI update
  auto domElement = menuPtr->createSDomElement(&app);

  menuPtr->setDisabled(true);

  BOOST_REQUIRE(menuPtr->isDisabled());
  BOOST_REQUIRE(menuPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(menuItem->hasStyleClass("Wt-disabled"));

  menuPtr->setDisabled(false);

  BOOST_REQUIRE(!menuPtr->isDisabled());
  BOOST_REQUIRE(!menuPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(!menuItem->hasStyleClass("Wt-disabled"));

  menuItem->setDisabled(true);

  BOOST_REQUIRE(!menuPtr->isDisabled());
  BOOST_REQUIRE(!menuPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(menuItem->isDisabled());
  BOOST_REQUIRE(menuItem->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WMenu_setDisabled_Bootstrap2 )
{
  // Ensures that when a WMenu is disabled its menu items are visually disabled as well (but not in code).
  // If the WMenuItem alone is disabled, the WMenu parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap2Theme>());

  auto menu = std::make_unique<Wt::WMenu>();
  auto menuItem = menu->addItem("Item 1");
  auto menuPtr = menu.get();
  app.root()->addWidget(std::move(menu));

  // Simulate UI update
  auto domElement =  menuPtr->createSDomElement(&app);

  menuPtr->setDisabled(true);

  BOOST_REQUIRE(menuPtr->isDisabled());
  BOOST_REQUIRE(menuPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(menuItem->hasStyleClass("disabled"));

  menuPtr->setDisabled(false);

  BOOST_REQUIRE(!menuPtr->isDisabled());
  BOOST_REQUIRE(!menuPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(!menuItem->hasStyleClass("disabled"));

  menuItem->setDisabled(true);

  BOOST_REQUIRE(!menuPtr->isDisabled());
  BOOST_REQUIRE(!menuPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(menuItem->isDisabled());
  BOOST_REQUIRE(menuItem->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WMenu_addItem_when_setDisabled )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto menu = std::make_unique<Wt::WMenu>();
  auto menuItem = menu->addItem("Item 1");
  auto menuPtr = menu.get();
  app.root()->addWidget(std::move(menu));

  // Simulate UI update
  auto domElement1=  menuPtr->createSDomElement(&app);

  menuPtr->setDisabled(true);

  menuItem = menuPtr->addItem("Item 2");

  // Add the item to the DOM
  auto domElement2 =  menuPtr->createSDomElement(&app);

  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(menuItem->hasStyleClass("Wt-disabled"));

  menuPtr->setDisabled(false);

  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(!menuItem->hasStyleClass("Wt-disabled"));

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WMenu_addItem_when_setDisabled_Bootstrap3 )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap3Theme>());

  auto menu = std::make_unique<Wt::WMenu>();
  auto menuItem = menu->addItem("Item 1");
  auto menuPtr = menu.get();
  app.root()->addWidget(std::move(menu));

  // Simulate UI update
  auto domElement1 =  menuPtr->createSDomElement(&app);

  menuPtr->setDisabled(true);

  menuItem = menuPtr->addItem("Item 2");

  // Add the item to the DOM
  auto domElement2 =  menuPtr->createSDomElement(&app);

  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(menuItem->hasStyleClass("disabled"));

  menuPtr->setDisabled(false);

  BOOST_REQUIRE(!menuItem->isDisabled());
  BOOST_REQUIRE(!menuItem->hasStyleClass("disabled"));

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTabWidget_setDisabled )
{
  // Ensures that when a WTabWidget is disabled its tab items are visually disabled as well (but not in code).
  // If the WMenuItem (one tab) alone is disabled, the WTabWidget parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto tab = std::make_unique<Wt::WTabWidget>();
  auto tabItem = tab->addTab(std::make_unique<Wt::WText>("Item 1"), "Item 1");
  auto tabPtr = tab.get();
  app.root()->addWidget(std::move(tab));

  // Simulate UI update
  auto domElement =  tabPtr->createSDomElement(&app);

  tabPtr->setDisabled(true);

  BOOST_REQUIRE(tabPtr->isDisabled());
  BOOST_REQUIRE(tabPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(tabItem->hasStyleClass("Wt-disabled"));

  tabPtr->setDisabled(false);

  BOOST_REQUIRE(!tabPtr->isDisabled());
  BOOST_REQUIRE(!tabPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(!tabItem->hasStyleClass("Wt-disabled"));

  tabItem->setDisabled(true);

  BOOST_REQUIRE(!tabPtr->isDisabled());
  BOOST_REQUIRE(!tabPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(tabItem->isDisabled());
  BOOST_REQUIRE(tabItem->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTabWidget_setDisabled_Bootstrap5 )
{
  // Ensures that when a WTabWidget is disabled its tab items are visually disabled as well (but not in code).
  // If the WMenuItem (one tab) alone is disabled, the WTabWidget parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap5Theme>());

  auto tab = std::make_unique<Wt::WTabWidget>();
  auto tabItem = tab->addTab(std::make_unique<Wt::WText>("Item 1"), "Item 1");
  auto tabPtr = tab.get();
  app.root()->addWidget(std::move(tab));

  // Simulate UI update
  auto domElement =  tabPtr->createSDomElement(&app);

  tabPtr->setDisabled(true);

  BOOST_REQUIRE(tabPtr->isDisabled());
  BOOST_REQUIRE(tabPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(tabItem->hasStyleClass("disabled"));

  tabPtr->setDisabled(false);

  BOOST_REQUIRE(!tabPtr->isDisabled());
  BOOST_REQUIRE(!tabPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(!tabItem->hasStyleClass("disabled"));

  tabItem->setDisabled(true);

  BOOST_REQUIRE(!tabPtr->isDisabled());
  BOOST_REQUIRE(!tabPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(tabItem->isDisabled());
  BOOST_REQUIRE(tabItem->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTabWidget_addTab_when_setDisabled )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto tab = std::make_unique<Wt::WTabWidget>();
  auto tabItem = tab->addTab(std::make_unique<Wt::WText>("Item 1"), "Item 1");
  auto tabPtr = tab.get();
  app.root()->addWidget(std::move(tab));

  // Simulate UI update
  auto domElement1 =  tabPtr->createSDomElement(&app);

  tabPtr->setDisabled(true);

  tabItem = tabPtr->addTab(std::make_unique<Wt::WText>("Item 2"), "Item 2");

  // Add the item to the DOM
  auto domElement2 =  tabPtr->createSDomElement(&app);

  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(tabItem->hasStyleClass("Wt-disabled"));

  tabPtr->setDisabled(false);

  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(!tabItem->hasStyleClass("Wt-disabled"));

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTabWidget_addTab_when_setDisabled_Bootstrap2 )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap2Theme>());

  auto tab = std::make_unique<Wt::WTabWidget>();
  auto tabItem = tab->addTab(std::make_unique<Wt::WText>("Item 1"), "Item 1");
  auto tabPtr = tab.get();
  app.root()->addWidget(std::move(tab));

  // Simulate UI update
  auto domElement1 =  tabPtr->createSDomElement(&app);

  tabPtr->setDisabled(true);

  tabItem = tabPtr->addTab(std::make_unique<Wt::WText>("Item 2"), "Item 2");

  // Add the item to the DOM
  auto domElement2 =  tabPtr->createSDomElement(&app);

  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(tabItem->hasStyleClass("disabled"));

  tabPtr->setDisabled(false);

  BOOST_REQUIRE(!tabItem->isDisabled());
  BOOST_REQUIRE(!tabItem->hasStyleClass("disabled"));

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WToolBar_setDisabled )
{
  // Ensures that when a WToolBar is disabled its button items are visually disabled as well (but not in code).
  // If the WPushButton alone is disabled, the WToolBar parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto toolbar = std::make_unique<Wt::WToolBar>();
  toolbar->addButton(std::make_unique<Wt::WPushButton>("Item 1"));
  auto toolbarPtr = toolbar.get();
  app.root()->addWidget(std::move(toolbar));

  // Simulate UI update
  auto domElement =  toolbarPtr->createSDomElement(&app);

  toolbarPtr->setDisabled(true);

  BOOST_REQUIRE(toolbarPtr->isDisabled());
  BOOST_REQUIRE(toolbarPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!toolbarPtr->widget(0)->isDisabled());
  BOOST_REQUIRE(toolbarPtr->widget(0)->hasStyleClass("Wt-disabled"));

  toolbarPtr->setDisabled(false);

  BOOST_REQUIRE(!toolbarPtr->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!toolbarPtr->widget(0)->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->widget(0)->hasStyleClass("Wt-disabled"));

  toolbarPtr->widget(0)->setDisabled(true);

  BOOST_REQUIRE(!toolbarPtr->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(toolbarPtr->widget(0)->isDisabled());
  BOOST_REQUIRE(toolbarPtr->widget(0)->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WToolBar_setDisabled_Bootstrap3 )
{
  // Ensures that when a WToolBar is disabled its button items are visually disabled as well (but not in code).
  // If the WPushButton alone is disabled, the WToolBar parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap3Theme>());

  auto toolbar = std::make_unique<Wt::WToolBar>();
  toolbar->addButton(std::make_unique<Wt::WPushButton>("Item 1"));
  auto toolbarPtr = toolbar.get();
  app.root()->addWidget(std::move(toolbar));

  // Simulate UI update
  auto domElement =  toolbarPtr->createSDomElement(&app);

  toolbarPtr->setDisabled(true);

  BOOST_REQUIRE(toolbarPtr->isDisabled());
  BOOST_REQUIRE(toolbarPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!toolbarPtr->widget(0)->isDisabled());
  BOOST_REQUIRE(toolbarPtr->widget(0)->hasStyleClass("disabled"));

  toolbarPtr->setDisabled(false);

  BOOST_REQUIRE(!toolbarPtr->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!toolbarPtr->widget(0)->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->widget(0)->hasStyleClass("disabled"));

  toolbarPtr->widget(0)->setDisabled(true);

  BOOST_REQUIRE(!toolbarPtr->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(toolbarPtr->widget(0)->isDisabled());
  BOOST_REQUIRE(toolbarPtr->widget(0)->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WToolBar_addButton_when_setDisabled )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto toolbar = std::make_unique<Wt::WToolBar>();
  toolbar->addButton(std::make_unique<Wt::WPushButton>("Item 1"));
  auto toolbarPtr = toolbar.get();
  app.root()->addWidget(std::move(toolbar));

  // Simulate UI update
  auto domElement1 =  toolbarPtr->createSDomElement(&app);

  toolbarPtr->setDisabled(true);

  toolbarPtr->addButton(std::make_unique<Wt::WPushButton>("Item 2"));

  // Add the item to the DOM
  auto domElement2 =  toolbarPtr->createSDomElement(&app);

  BOOST_REQUIRE(!toolbarPtr->widget(1)->isDisabled());
  BOOST_REQUIRE(toolbarPtr->widget(1)->hasStyleClass("Wt-disabled"));

  toolbarPtr->setDisabled(false);

  BOOST_REQUIRE(!toolbarPtr->widget(1)->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->widget(1)->hasStyleClass("Wt-disabled"));

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WToolBar_addButton_when_setDisabled_Bootstrap5 )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap5Theme>());

  auto toolbar = std::make_unique<Wt::WToolBar>();
  toolbar->addButton(std::make_unique<Wt::WPushButton>("Item 1"));
  auto toolbarPtr = toolbar.get();
  app.root()->addWidget(std::move(toolbar));

  // Simulate UI update
  auto domElement1 =  toolbarPtr->createSDomElement(&app);

  toolbarPtr->setDisabled(true);

  toolbarPtr->addButton(std::make_unique<Wt::WPushButton>("Item 2"));

  // Add the item to the DOM
  auto domElement2 =  toolbarPtr->createSDomElement(&app);

  BOOST_REQUIRE(!toolbarPtr->widget(1)->isDisabled());
  BOOST_REQUIRE(toolbarPtr->widget(1)->hasStyleClass("disabled"));

  toolbarPtr->setDisabled(false);

  BOOST_REQUIRE(!toolbarPtr->widget(1)->isDisabled());
  BOOST_REQUIRE(!toolbarPtr->widget(1)->hasStyleClass("disabled"));

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeView_setDisabled )
{
  // Ensures that when a WTreeView is disabled its items are visually disabled as well (but not in code).
  // If the item alone is disabled, the WTreeView parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto treeview = std::make_unique<Wt::WTreeView>();
  treeview->setModel(model);
  auto treeviewPtr = treeview.get();
  app.root()->addWidget(std::move(treeview));

  // Simulate UI update
  auto domElement =  treeviewPtr->createSDomElement(&app);

  treeviewPtr->setDisabled(true);

  BOOST_REQUIRE(treeviewPtr->isDisabled());
  BOOST_REQUIRE(treeviewPtr->hasStyleClass("Wt-disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeviewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(treeviewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  treeviewPtr->setDisabled(false);

  BOOST_REQUIRE(!treeviewPtr->isDisabled());
  BOOST_REQUIRE(!treeviewPtr->hasStyleClass("Wt-disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeviewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!treeviewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  treeviewPtr->itemWidget(model->index(0))->setDisabled(true);

  BOOST_REQUIRE(!treeviewPtr->isDisabled());
  BOOST_REQUIRE(!treeviewPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(treeviewPtr->itemWidget(model->index(0))->isDisabled());
  BOOST_REQUIRE(treeviewPtr->itemWidget(model->index(0))->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeView_setDisabled_Bootstrap2 )
{
  // Ensures that when a WTreeView is disabled its items are visually disabled as well (but not in code).
  // If the item alone is disabled, the WTreeView parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap2Theme>());

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto treeview = std::make_unique<Wt::WTreeView>();
  treeview->setModel(model);
  auto treeviewPtr = treeview.get();
  app.root()->addWidget(std::move(treeview));

  // Simulate UI update
  auto domElement =  treeviewPtr->createSDomElement(&app);

  treeviewPtr->setDisabled(true);

  BOOST_REQUIRE(treeviewPtr->isDisabled());
  BOOST_REQUIRE(treeviewPtr->hasStyleClass("disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeviewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(treeviewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  treeviewPtr->setDisabled(false);

  BOOST_REQUIRE(!treeviewPtr->isDisabled());
  BOOST_REQUIRE(!treeviewPtr->hasStyleClass("disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeviewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!treeviewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  treeviewPtr->itemWidget(model->index(0))->setDisabled(true);

  BOOST_REQUIRE(!treeviewPtr->isDisabled());
  BOOST_REQUIRE(!treeviewPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(treeviewPtr->itemWidget(model->index(0))->isDisabled());
  BOOST_REQUIRE(treeviewPtr->itemWidget(model->index(0))->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeView_setModel_when_setDisabled )
{
  // Ensures that when a WTreeView is disabled, replacing the model keeps it disabled .

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto treeView = std::make_unique<Wt::WTreeView>();
  treeView->setModel(model);
  auto treeViewPtr = treeView.get();
  app.root()->addWidget(std::move(treeView));

  // Simulate UI update
  auto domElement1 =  treeViewPtr->createSDomElement(&app);

  treeViewPtr->setDisabled(true);

  treeViewPtr->setModel(model);

  // This actually needs to call a rerender, to ensure that the items are not
  // constructed again, but updated, leading to an update of itrs root node.
  auto domElement2 =  treeViewPtr->createSDomElement(&app);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(treeViewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  treeViewPtr->setDisabled(false);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!treeViewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeView_setModel_when_setDisabled_Bootstrap3 )
{
  // Ensures that when a WTreeView is disabled, replacing the model keeps it disabled .

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap3Theme>());

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto treeView = std::make_unique<Wt::WTreeView>();
  treeView->setModel(model);
  auto treeViewPtr = treeView.get();
  app.root()->addWidget(std::move(treeView));

  // Simulate UI update
  auto domElement1 =  treeViewPtr->createSDomElement(&app);

  treeViewPtr->setDisabled(true);

  treeViewPtr->setModel(model);

  // This actually needs to call a rerender, to ensure that the items are not
  // constructed again, but updated, leading to an update of itrs root node.
  auto domElement2 =  treeViewPtr->createSDomElement(&app);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(treeViewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  treeViewPtr->setDisabled(false);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!treeViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!treeViewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTableView_setDisabled )
{
  // Ensures that when a WTableView is disabled its items are visually disabled as well (but not in code).
  // If the item alone is disabled, the WTableView parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto tableView = std::make_unique<Wt::WTableView>();
  tableView->setModel(model);
  auto tableViewPtr = tableView.get();
  app.root()->addWidget(std::move(tableView));

  // Simulate UI update
  auto domElement =  tableViewPtr->createSDomElement(&app);

  tableViewPtr->setDisabled(true);

  BOOST_REQUIRE(tableViewPtr->isDisabled());
  BOOST_REQUIRE(tableViewPtr->hasStyleClass("Wt-disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  tableViewPtr->setDisabled(false);

  BOOST_REQUIRE(!tableViewPtr->isDisabled());
  BOOST_REQUIRE(!tableViewPtr->hasStyleClass("Wt-disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  tableViewPtr->itemWidget(model->index(0))->setDisabled(true);

  BOOST_REQUIRE(!tableViewPtr->isDisabled());
  BOOST_REQUIRE(!tableViewPtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(0))->isDisabled());
  BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(0))->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTableView_setDisabled_Bootstrap5 )
{
  // Ensures that when a WTableView is disabled its items are visually disabled as well (but not in code).
  // If the item alone is disabled, the WTableView parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap5Theme>());

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto tableView = std::make_unique<Wt::WTableView>();
  tableView->setModel(model);
  auto tableViewPtr = tableView.get();
  app.root()->addWidget(std::move(tableView));

  // Simulate UI update
  auto domElement =  tableViewPtr->createSDomElement(&app);

  tableViewPtr->setDisabled(true);

  BOOST_REQUIRE(tableViewPtr->isDisabled());
  BOOST_REQUIRE(tableViewPtr->hasStyleClass("disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  tableViewPtr->setDisabled(false);

  BOOST_REQUIRE(!tableViewPtr->isDisabled());
  BOOST_REQUIRE(!tableViewPtr->hasStyleClass("disabled"));
  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  tableViewPtr->itemWidget(model->index(0))->setDisabled(true);

  BOOST_REQUIRE(!tableViewPtr->isDisabled());
  BOOST_REQUIRE(!tableViewPtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(0))->isDisabled());
  BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(0))->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTableView_setModel_when_setDisabled )
{
  // Ensures that when a WTableView is disabled, replacing the model keeps it disabled .

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto tableView = std::make_unique<Wt::WTableView>();
  tableView->setModel(model);
  auto tableViewPtr = tableView.get();
  app.root()->addWidget(std::move(tableView));

  // Simulate UI update
  auto domElement1 =  tableViewPtr->createSDomElement(&app);

  tableViewPtr->setDisabled(true);

  tableViewPtr->setModel(model);

  // "Refresh" the widget
  auto domElement2 =  tableViewPtr->createSDomElement(&app);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  tableViewPtr->setDisabled(false);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->hasStyleClass("Wt-disabled"));
  }

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTableView_setModel_when_setDisabled_Bootstrap2 )
{
  // Ensures that when a WTableView is disabled, replacing the model keeps it disabled .

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap2Theme>());

  auto model = std::make_shared<Wt::WStringListModel>();
  std::vector<Wt::WString> data = { "Item 1", "Item 2", "Item 3" };
  model->setStringList(data);

  auto tableView = std::make_unique<Wt::WTableView>();
  tableView->setModel(model);
  auto tableViewPtr = tableView.get();
  app.root()->addWidget(std::move(tableView));

  // Simulate UI update
  auto domElement1 =  tableViewPtr->createSDomElement(&app);

  tableViewPtr->setDisabled(true);

  tableViewPtr->setModel(model);

  // "Refresh" the widget
  auto domElement2 =  tableViewPtr->createSDomElement(&app);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(tableViewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  tableViewPtr->setDisabled(false);

  for (int index = 0; index < model->rowCount(); ++index) {
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->isDisabled());
    BOOST_REQUIRE(!tableViewPtr->itemWidget(model->index(index))->hasStyleClass("disabled"));
  }

  delete domElement1;
  delete domElement2;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTree_setDisabled )
{
  // Ensures that when a WTree is disabled its WTreeNode items are visually disabled as well (but not in code).
  // If the WTreeNode item alone is disabled, the WTree parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto tree = std::make_unique<Wt::WTree>();
  tree->setTreeRoot(std::make_unique<Wt::WTreeNode>("root"));
  tree->treeRoot()->addChildNode(std::make_unique<Wt::WTreeNode>("Item 1"));
  auto treePtr = tree.get();
  app.root()->addWidget(std::move(tree));

  // Make sure the items are showing
  treePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treePtr->createSDomElement(&app);

  treePtr->setDisabled(true);

  BOOST_REQUIRE(treePtr->isDisabled());
  BOOST_REQUIRE(treePtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[0]->hasStyleClass("Wt-disabled"));

  treePtr->setDisabled(false);

  BOOST_REQUIRE(!treePtr->isDisabled());
  BOOST_REQUIRE(!treePtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[0]->hasStyleClass("Wt-disabled"));

  treePtr->treeRoot()->childNodes()[0]->setDisabled(true);

  BOOST_REQUIRE(!treePtr->isDisabled());
  BOOST_REQUIRE(!treePtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[0]->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTree_setDisabled_Bootstrap3 )
{
  // Ensures that when a WTree is disabled its WTreeNode items are visually disabled as well (but not in code).
  // If the WTreeNode item alone is disabled, the WTree parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap3Theme>());

  auto tree = std::make_unique<Wt::WTree>();
  tree->setTreeRoot(std::make_unique<Wt::WTreeNode>("root"));
  tree->treeRoot()->addChildNode(std::make_unique<Wt::WTreeNode>("Item 1"));
  auto treePtr = tree.get();
  app.root()->addWidget(std::move(tree));

  // Make sure the items are showing
  treePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treePtr->createSDomElement(&app);

  treePtr->setDisabled(true);

  BOOST_REQUIRE(treePtr->isDisabled());
  BOOST_REQUIRE(treePtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[0]->hasStyleClass("disabled"));

  treePtr->setDisabled(false);

  BOOST_REQUIRE(!treePtr->isDisabled());
  BOOST_REQUIRE(!treePtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[0]->hasStyleClass("disabled"));

  treePtr->treeRoot()->childNodes()[0]->setDisabled(true);

  BOOST_REQUIRE(!treePtr->isDisabled());
  BOOST_REQUIRE(!treePtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[0]->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTree_addChildNode_when_setDisabled )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto tree = std::make_unique<Wt::WTree>();
  tree->setTreeRoot(std::make_unique<Wt::WTreeNode>("root"));
  tree->treeRoot()->addChildNode(std::make_unique<Wt::WTreeNode>("Item 1"));
  auto treePtr = tree.get();
  app.root()->addWidget(std::move(tree));

  // Make sure the items are showing
  treePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treePtr->createSDomElement(&app);

  treePtr->setDisabled(true);

  treePtr->treeRoot()->addChildNode(std::make_unique<Wt::WTreeNode>("Item 2"));

  // Do not update DOM, this is done through WTreeNode::update()

  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[1]->hasStyleClass("Wt-disabled"));

  treePtr->setDisabled(false);

  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[1]->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTree_addChildNode_when_setDisabled_Bootstrap5 )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap5Theme>());

  auto tree = std::make_unique<Wt::WTree>();
  tree->setTreeRoot(std::make_unique<Wt::WTreeNode>("root"));
  tree->treeRoot()->addChildNode(std::make_unique<Wt::WTreeNode>("Item 1"));
  auto treePtr = tree.get();
  app.root()->addWidget(std::move(tree));

  // Make sure the items are showing
  treePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treePtr->createSDomElement(&app);

  treePtr->setDisabled(true);

  treePtr->treeRoot()->addChildNode(std::make_unique<Wt::WTreeNode>("Item 2"));

  // Do not update DOM, this is done through WTreeNode::update()

  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(treePtr->treeRoot()->childNodes()[1]->hasStyleClass("disabled"));

  treePtr->setDisabled(false);

  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(!treePtr->treeRoot()->childNodes()[1]->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeTable_setDisabled )
{
  // Ensures that when a WTree is disabled its WTreeNode items are visually disabled as well (but not in code).
  // If the WTreeNode item alone is disabled, the WTree parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto treeTable = std::make_unique<Wt::WTreeTable>();
  treeTable->setTreeRoot(std::make_unique<Wt::WTreeTableNode>("root"), "root");
  treeTable->treeRoot()->addChildNode(std::make_unique<Wt::WTreeTableNode>("Item 1"));
  auto treeTablePtr = treeTable.get();
  app.root()->addWidget(std::move(treeTable));

  // Make sure the items are showing
  treeTablePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treeTablePtr->createSDomElement(&app);

  treeTablePtr->setDisabled(true);

  BOOST_REQUIRE(treeTablePtr->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[0]->hasStyleClass("Wt-disabled"));

  treeTablePtr->setDisabled(false);

  BOOST_REQUIRE(!treeTablePtr->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[0]->hasStyleClass("Wt-disabled"));

  treeTablePtr->treeRoot()->childNodes()[0]->setDisabled(true);

  BOOST_REQUIRE(!treeTablePtr->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->hasStyleClass("Wt-disabled"));
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[0]->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeTable_setDisabled_Bootstrap2 )
{
  // Ensures that when a WTree is disabled its WTreeNode items are visually disabled as well (but not in code).
  // If the WTreeNode item alone is disabled, the WTree parent is not affected.

  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap2Theme>());

  auto treeTable = std::make_unique<Wt::WTreeTable>();
  treeTable->setTreeRoot(std::make_unique<Wt::WTreeTableNode>("root"), "root");
  treeTable->treeRoot()->addChildNode(std::make_unique<Wt::WTreeTableNode>("Item 1"));
  auto treeTablePtr = treeTable.get();
  app.root()->addWidget(std::move(treeTable));

  // Make sure the items are showing
  treeTablePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treeTablePtr->createSDomElement(&app);

  treeTablePtr->setDisabled(true);

  BOOST_REQUIRE(treeTablePtr->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[0]->hasStyleClass("disabled"));

  treeTablePtr->setDisabled(false);

  BOOST_REQUIRE(!treeTablePtr->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[0]->hasStyleClass("disabled"));

  treeTablePtr->treeRoot()->childNodes()[0]->setDisabled(true);

  BOOST_REQUIRE(!treeTablePtr->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->hasStyleClass("disabled"));
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[0]->isDisabled());
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[0]->hasStyleClass("disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeTable_addChildNode_when_setDisabled )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);

  auto treeTable = std::make_unique<Wt::WTreeTable>();
  treeTable->setTreeRoot(std::make_unique<Wt::WTreeTableNode>("root"), "root");
  treeTable->treeRoot()->addChildNode(std::make_unique<Wt::WTreeTableNode>("Item 1"));
  auto treeTablePtr = treeTable.get();
  app.root()->addWidget(std::move(treeTable));

  // Make sure the items are showing
  treeTablePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treeTablePtr->createSDomElement(&app);

  treeTablePtr->setDisabled(true);

  treeTablePtr->treeRoot()->addChildNode(std::make_unique<Wt::WTreeTableNode>("Item 2"));

  // Do not update DOM, this is done through WTreeNode::update()

  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[1]->hasStyleClass("Wt-disabled"));

  treeTablePtr->setDisabled(false);

  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[1]->hasStyleClass("Wt-disabled"));

  delete domElement;
}

BOOST_AUTO_TEST_CASE( WCompositeWidget_WTreeTable_addChildNode_when_setDisabled_Bootstrap3 )
{
  Wt::Test::WTestEnvironment environment;
  Wt::WApplication app(environment);
  app.setTheme(std::make_shared<Wt::WBootstrap3Theme>());

  auto treeTable = std::make_unique<Wt::WTreeTable>();
  treeTable->setTreeRoot(std::make_unique<Wt::WTreeTableNode>("root"), "root");
  treeTable->treeRoot()->addChildNode(std::make_unique<Wt::WTreeTableNode>("Item 1"));
  auto treeTablePtr = treeTable.get();
  app.root()->addWidget(std::move(treeTable));

  // Make sure the items are showing
  treeTablePtr->treeRoot()->expand();
  // Simulate UI update
  auto domElement =  treeTablePtr->createSDomElement(&app);

  treeTablePtr->setDisabled(true);

  treeTablePtr->treeRoot()->addChildNode(std::make_unique<Wt::WTreeTableNode>("Item 2"));

  // Do not update DOM, this is done through WTreeNode::update()

  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(treeTablePtr->treeRoot()->childNodes()[1]->hasStyleClass("disabled"));

  treeTablePtr->setDisabled(false);

  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[1]->isDisabled());
  BOOST_REQUIRE(!treeTablePtr->treeRoot()->childNodes()[1]->hasStyleClass("disabled"));

  delete domElement;
}

