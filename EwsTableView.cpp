#include "EwsTableView.h"

#include <QDebug>
#include <QMouseEvent>




EwsTableView::EwsTableView(QWidget *parent)
    : QTableView(parent)
{
}

void EwsTableView::init_horizontal_header()
{
    pHeader = new MultiLevelHeaderView(Qt::Horizontal, header_row_num, header_col_num, this);

    if (pHeader->tool_list.isEmpty())
    {
        for (int i = 0; i < header_row_num; ++i)
            for (int j = 0; j < header_col_num; ++j)
            {
                pHeader->setCellSpan(i, j, 1, 1);
            }
        pHeader->setCellText(0, 0, "No Tools");
    }
    else
    {
        // 表头内容布局
        pHeader->setCellSpan(0, 0, 1, 4);
        pHeader->setCellSpan(0, 4, 1, 4);

        pHeader->setCellSpan(1, 0, 1, 2);
        pHeader->setCellSpan(1, 2, 1, 2);
        pHeader->setCellSpan(1, 4, 1, 2);
        pHeader->setCellSpan(1, 6, 1, 2);

        // 一级
        pHeader->setCellText(0, 0, "eDevice");
        pHeader->setCellText(0, 4, "eVisual");
        // 二级
        pHeader->setCellText(1, 0, "band");
        pHeader->setCellText(1, 2, "mobility");
        pHeader->setCellText(1, 4, "traps");
        pHeader->setCellText(1, 6, "conc");
    }

    pHeader->setMinimumHeight(97);

    // 设置表头文本对齐方式
    // pHeader->setDefaultAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // for (int i = 0; i < 3; ++i)
    //   pHeader->setRowHeight(i, 128);

    // modify by hqh
    pHeader->setSectionsClickable(true);

    // pHeader->setCellBackgroundColor(0, 0, 0xcfcfcf);
    // pHeader->setCellBackgroundColor(0, 4, 0xcfcfcf);
    // pHeader->setCellBackgroundColor(1, 0, 0xcfcfcf);
    // pHeader->setCellBackgroundColor(1, 2, 0xcfcfcf);
    // pHeader->setCellBackgroundColor(1, 4, 0xcfcfcf);
    // pHeader->setCellBackgroundColor(1, 6, 0xcfcfcf);

    // for (int i = 0; i < 8; i++)
    // {
    //     pHeader->setCellBackgroundColor(2, i, 0xcfcfcf);
    // }

    int rowCount = 10;
    m_pDataModel = new QStandardItemModel;

    for (int i = 0; i < rowCount; i++)
    {
        QList<QStandardItem *> items;
        for (int j = 0; j < header_col_num; j++)
        {
            items.append(new QStandardItem);
        }

        m_pDataModel->appendRow(items);
    }

    setModel(m_pDataModel);
    setHorizontalHeader(pHeader);
    connect(pHeader, SIGNAL(header_add_tool(QString, int)), this, SLOT(add_tool(QString, int)));

    connect(pHeader, SIGNAL(header_add_param(int, QString, QString, int)), this, SLOT(add_param(int, QString, QString, int)));
}

void EwsTableView::init_vertical_header()
{
    auto pHeader = new MultiLevelHeaderView(Qt::Vertical, 8, 3, this);
    pHeader->setCellSpan(0, 0, 4, 1);
    pHeader->setCellSpan(4, 0, 4, 1);
    pHeader->setCellSpan(0, 1, 2, 1);
    pHeader->setCellSpan(2, 1, 2, 1);
    pHeader->setCellSpan(4, 1, 2, 1);
    pHeader->setCellSpan(6, 1, 2, 1);
    for (int i = 0; i < 8; i++)
    {
        pHeader->setCellSpan(i, 2, 1, 1);
    }

    // 一级
    pHeader->setCellText(0, 0, QString(u8"横\n向\n尺\n寸"));
    pHeader->setCellText(4, 0, QString(u8"纵\n向\n尺\n寸"));
    // 二级
    pHeader->setCellText(0, 1, QStringLiteral("极耳宽度"));
    pHeader->setCellText(2, 1, QStringLiteral("极耳高度"));
    pHeader->setCellText(4, 1, QStringLiteral("极片宽度"));
    pHeader->setCellText(6, 1, QStringLiteral("极耳间距"));
    // 三级
    pHeader->setCellText(0, 2, QStringLiteral("CCD测量值"));
    pHeader->setCellText(1, 2, QStringLiteral("真值"));
    pHeader->setCellText(2, 2, QStringLiteral("CCD测量值"));
    pHeader->setCellText(3, 2, QStringLiteral("真值"));
    pHeader->setCellText(4, 2, QStringLiteral("CCD测量值"));
    pHeader->setCellText(5, 2, QStringLiteral("真值"));
    pHeader->setCellText(6, 2, QStringLiteral("CCD测量值"));
    pHeader->setCellText(7, 2, QStringLiteral("真值"));

    pHeader->setMinimumHeight(90);
    pHeader->setMinimumWidth(30);
    for (int i = 0; i < 3; ++i)
        pHeader->setColumnWidth(i, 30);
    for (int i = 0; i < 8; ++i)
        pHeader->setRowHeight(i, 30);
    pHeader->setSectionsClickable(false);

    int rowCount = 8;
    auto m_pDataModel = new QStandardItemModel;
    for (int i = 0; i < rowCount; i++)
    {
        QList<QStandardItem *> items;
        for (int j = 0; j < 8; j++)
        {
            items.append(new QStandardItem);
        }

        m_pDataModel->appendRow(items);
    }

    setModel(m_pDataModel);
    setVerticalHeader(pHeader);
}

void EwsTableView::init_table_header()
{
    init_horizontal_header();
}

void EwsTableView::add_tool(QString tool_name, int pos)
{
    // fix to do 刷新数据模型
    ToolNode *tool_node = new ToolNode(tool_name, pos);
    tool_list.insert(tool_node->pos, tool_node);
    tool_node->params_list.append("#");
    // fix to do根据tool_list数据模型刷新UI
    pHeader->setCellText(0, tool_node->pos, tool_node->name);

    QModelIndex index = m_pDataModel->index(0, 0);
    m_pDataModel->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    m_pDataModel->setData(index, "--", Qt::EditRole);
    // 获取单元格内容
    QString data = m_pDataModel->data(index).toString();
    qInfo() << "EwsTableView add_tool done";
}

void EwsTableView::add_param(int tool_col, QString param_key, QString value_list, int param_pos)
{
    // fix to do 刷新数据模型
    // ToolNode *tool_node = new ToolNode(tool_name, pos);
    // tool_list.insert(tool_node->pos, tool_node);

    // fix to do根据tool_list数据模型刷新UI
    // pHeader->setCellText(0, tool_node->pos, tool_node->name);
    int insert_pos = 0;
    //for (int i = 0; i < ptool_node->pos; i++)
    //{
    //    insert_pos+= tool_list[i]->params_list.length();
    // }
    pHeader->setCellText(1, insert_pos + param_pos, param_key);
    tool_list[0]->params_list.append(param_key);
    // 通过tool_col 计算给哪个点工具添加参数
    int start_pos = 0;
    ToolNode* ptool_node = nullptr;
    for(auto ptool:tool_list)
    {
        start_pos += ptool->params_list.length();
        if (start_pos >= tool_col)
        {
            ptool_node = ptool;
            break;
        }
    }
    for (int i = 0; i < ptool_node->pos; i++)
    {
        insert_pos+= tool_list[i]->params_list.length();
    }
    QModelIndex index = m_pDataModel->index(0, insert_pos + param_pos);
    m_pDataModel->item(0, insert_pos + param_pos)->setTextAlignment(Qt::AlignCenter);
    m_pDataModel->setData(index, value_list, Qt::EditRole);
    pHeader->setCellSpan(0, 0, 1, tool_list[0]->params_list.length());
    // 获取单元格内容
    QString data = m_pDataModel->data(index).toString();
    qInfo() << "EwsTableView add_param done";
}

// void EwsTableView::on_clicked(const QModelIndex& idx)
// {
//     qInfo() << "ttttttttttttttttttt on_clicked row:" << idx.row() << ",col:"<< idx.column();
// }

void EwsTableView::popup_tool_menu(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    if (!index.isValid())
    {
        qDebug() << "Current QModelIndex is not valid!";
        return;
    }

    _tool_menu.exec(viewport()->mapToGlobal(pos));
}

// void EwsTableView::mousePressEvent(QMouseEvent* event)
// {
//     if (event->button() == Qt::LeftButton)
//     {
//         QTableView::mousePressEvent(event);
//         // 左键点击的单元格
//         QModelIndex index = currentIndex();
//         // 处理左键事件

//     }
//     if (event->button() == Qt::RightButton)
//     {
//         QTableView::mousePressEvent(event);
//         QModelIndex index = currentIndex();
//         // 处理右键事件
//         qInfo() << "ttttttttttttttttttt row:" << index.row() << ",col:"<< index.column();

//     }

//     if (event->type() == QEvent::MouseButtonDblClick && event->button() == Qt::LeftButton)
//     {
//         QTableView::mousePressEvent(event);
//         QModelIndex index = currentIndex();
//         // 处理鼠标双击事件
//     }

// }
