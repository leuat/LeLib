#ifndef CINIFILE_H
#define CINIFILE_H

#include <QString>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QVector>


class CItem {
public:
    QString name, strval;
    QStringList lst;
    double dval;
};

class CIniFile  {
public:
    QString filename;

    QVector<CItem> items;

    void Load(QString fname);
    void Save(QString fname);

    QString getString(QString name) {
        for (int i=0;i<items.size();i++) {
            if (items[i].name==name.toLower().trimmed())
                return items[i].strval;
        }
        return "";
    }

    QStringList getStringList(QString name) {
        for (int i=0;i<items.size();i++) {
            if (items[i].name==name.toLower().trimmed())
                return items[i].lst;
        }
        return QStringList();
    }

    void setString(QString name, QString val) {
        for (int i=0;i<items.size();i++) {
            if (items[i].name==name.toLower().trimmed()) {
                items[i].strval = val;
                return;
            }

        }
        CItem i;
        i.name = name;
        i.strval  = val;
        items.append(i);

    }



    bool getBool(QString name) {
        for (int i=0;i<items.size();i++) {
            if (items[i].name==name.toLower().trimmed()) {
                if (items[i].strval=="true")
                    return true;
                return false;
            }
        }
        qDebug() << "CIniFile: Could not find parameter " + name;
    }

    int getInt(QString name) {
        for (int i=0;i<items.size();i++) {
            if (items[i].name==name.toLower().trimmed())
                return (int)items[i].dval;
        }
        qDebug() << "CIniFile: Could not find parameter " + name;
    }

    double getdouble(QString name) {
        for (int i=0;i<items.size();i++) {
            if (items[i].name==name.toLower().trimmed())
                return items[i].dval;
        }
        qDebug() << "CIniFile: Could not find parameter " + name;
    }

/*    bool find(QString key, QString value) {
        QQString actualValue = QQString::fromStdQString(getQString(key.toLower().toStdQString()));
        return value.toLower().compare(actualValue) == 0;
    }

    bool find(QString key, int value) {
        return getint(key.toStdQString()) == value;
    }

    bool find(QString key, bool value) {
        return getbool(key.toStdQString()) == value;
    }*/
};
#endif // CINIFILE_H
