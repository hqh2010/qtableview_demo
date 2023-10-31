#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <QList>
#include <QVariantMap>

class ToolNode
{
public:
    ToolNode(QString tool_name, int tool_pos=0):name(tool_name),pos(tool_pos)
    {
    }
    QString name;
    int pos;
    QStringList params_list;
    QVariantMap params_dict;

};

class ExperimentNode
{
public:
    int id;
    QString param_key;
    QString param_value;
    QString status;
};

class ExperimentTree
{
public:
    QList<ExperimentNode*> node_list;
};




#endif // DATA_MODEL_H
