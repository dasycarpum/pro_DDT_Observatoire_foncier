#include "territoire.h"


QStringList Territoire::Liste_libelles_geographies(QString const& granularite)
{
    QStringList liste;

    FichierCsv *f = new FichierCsv("/databank/territoire/" + granularite, ",");
    f->Lire();

    int champ_departement(3), champ_libelle(8);

    for (int i(1); i < f->matrix.size(); ++i)
        if (f->matrix[i][champ_departement] == "54")
            liste.append(f->matrix[i][champ_libelle]);

    delete f;

    return liste;
}

QStringList Territoire::Liste_codes_INSEE_communes(void)
{
    QStringList liste;


    return liste;
}
