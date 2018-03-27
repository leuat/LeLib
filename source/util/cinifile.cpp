#include "cinifile.h"


void CIniFile::Load(QString fname) {

    filename = fname;
    if (!QFile::exists(fname)) {
        qDebug() << "Could not find file " << fname;
        return;
    }
    qDebug() << fname << "exists";

    QFile file(fname);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream f(&file);

    while(!f.atEnd()) {

        QString line = f.readLine();



        QStringList tok = line.split("=");
        if (tok.size()==2) {
            CItem it;
            it.dval = -1;
            it.name = tok[0].toLower().trimmed();
            it.strval = tok[1].toLower().trimmed();
            it.dval  = tok[1].toFloat();
            if (tok[1].split(",").count()!=1) {
                it.strval = "";
                it.dval = -1;
                it.lst = tok[1].split(",");
                it.lst.removeFirst();
                for (QString& s : it.lst) {
                    s = s.trimmed();
                 //   qDebug() << s;
                }
            }
            items.push_back(it);
        }
    }
    file.close();
}

void CIniFile::Save(QString fname)
{
    if (QFile::exists(fname)) {
        QFile::remove(fname);
    }

    QFile file(fname);
    file.open(QIODevice::WriteOnly| QIODevice::Text);
    QTextStream f(&file);
    for (CItem i: items) {
        f << i.name << " = ";
        if (i.strval!="")
           f << i.strval << "\n";
        else
          if (i.lst.count()!=0) {
              f<<",";
              for (int j=0;j<i.lst.count();j++) {
                  f<<i.lst[j];
                 // qDebug() << " << " << i.lst[j];
                  if (j!=i.lst.count()-1)
                      f<<", ";
              }

          }
        else
            f << QString::number(i.dval) << "\n";

    }
    file.close();

}
