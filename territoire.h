#ifndef TERRITOIRE_H
#define TERRITOIRE_H

#include <QString>
#include <QPair>
#include <QStringList>

#include "fichier.h"

const int HA = 10000;

class Territoire
{
    const QString departement;          // numéro du département
    const QString granularite;          // plus petit grain = commune; sinon EPCI, SCoT, département
    QPair<QString, QString> geographie; // code, libellé
    QStringList liste_codes_INSEE_communes;
    QPair<QSet<QString>, QList<double>> emprise_communale; // Codes INSEE de(s) commune(s) du territoire
                                                           // ET coordonnées XY du rectangle
public:
    Territoire(QString const&, QString const&, QPair<QString, QString> const&);
    ~Territoire(){}

    QString Departement(void) const {return departement;}
    QString Granularite(void) const {return granularite;}
    QPair<QString, QString> Geographie(void) const {return geographie;}
    QPair<QSet<QString>, QList<double>> Emprise_communale(void) const {return emprise_communale;}

    QStringList Liste_codes_INSEE_communes(void) const {return liste_codes_INSEE_communes;}
    void Evaluation_emprise_communale(void);

    static QStringList Liste_libelles_geographies(QString const&, QString const&);

};

#endif // TERRITOIRE_H
