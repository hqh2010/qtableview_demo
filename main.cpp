#include <QApplication>

#include "EwsTableView.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EwsTableView tableView;
    tableView.setMinimumSize(800, 600);

    tableView.init_table_header();
    tableView.show();
    return a.exec();
}




