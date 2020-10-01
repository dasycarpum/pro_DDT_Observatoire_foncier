#ifndef TERRITOIRE_H
#define TERRITOIRE_H

#include <QString>
#include <QPair>
#include <QStringList>

#include "fichier.h"

class Territoire
{
    QString granularite;                // plus petit grain = commune; sinon EPCI, SCoT, département
    QPair<QString, QString> geographie; // code, libellé

public:
    Territoire(QString const& gra, QPair<QString, QString> const& geo) : granularite(gra), geographie(geo) {}
    ~Territoire(){}

    static QStringList Liste_libelles_geographies(QString const&);
    QStringList Liste_codes_INSEE_communes(void);
};

#endif // TERRITOIRE_H
