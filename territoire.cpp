#include "territoire.h"

Territoire::Territoire(QString const& gra, QPair<QString, QString> const& geo) : granularite(gra), geographie(geo)
{
    if (geographie.first.isEmpty()){

        /* N° de champ du libellé géographique pour la granularité choisie par l'utilisateur */
        int champ_lib_geo(0), champ_cod_com(0), champ_cod_dep(0);

        FichierCsv *g = new FichierCsv("/databank/territoire/granularite");
        g->Lire();

        for (int i(1); i < g->matrix.size(); ++i){
            if (g->matrix[i][0] == granularite)
                champ_lib_geo = g->matrix[i][2].toInt();
            if (g->matrix[i][0] == "Département")
                champ_cod_dep =  g->matrix[i][1].toInt();
            if (g->matrix[i][0] == "Commune")
                champ_cod_com =  g->matrix[i][1].toInt();
        }
        delete g;

        /* Liste des codes INSEE communaux du territoire */
        FichierCsv *cl = new FichierCsv("/databank/territoire/code_libelle");
        cl->Lire();

        for (int i(1); i < cl->matrix.size(); ++i)
            if (cl->matrix[i][champ_cod_dep] == DEPARTEMENT && cl->matrix[i][champ_lib_geo] == geographie.second)
                liste_codes_INSEE_communes.append(cl->matrix[i][champ_cod_com]);

        delete cl;   
    }
}

QStringList Territoire::Liste_libelles_geographies(QString const& granularite)
{
    QMap<QString, bool> liste_libelle;

    /* N° de champ des libellés géographiques pour la granularité choisie par l'utilisateur */
    int champ_lib_geo(0), champ_cod_dep(0);

    FichierCsv *g = new FichierCsv("/databank/territoire/granularite");
    g->Lire();

    for (int i(1); i < g->matrix.size(); ++i){
        if (g->matrix[i][0] == granularite)
            champ_lib_geo = g->matrix[i][2].toInt();
        if (g->matrix[i][0] == "Département")
            champ_cod_dep =  g->matrix[i][1].toInt();
    }
    delete g;

    /* Liste des libellés géographiques pour la granularité retenue */
    FichierCsv *cl = new FichierCsv("/databank/territoire/code_libelle");
    cl->Lire();

    for (int i(1); i < cl->matrix.size(); ++i)
        if (cl->matrix[i][champ_cod_dep] == DEPARTEMENT)
            liste_libelle[cl->matrix[i][champ_lib_geo]] = true;

    delete cl;


    return liste_libelle.keys();
}

/** Calcule les coordonnées XY du rectangle d'emprise maximale
    ========================================================== */
void Territoire::Evaluation_emprise_communale(void)
{
    /* Codes INSEE des communes du territoire */
    foreach (QString const& commune, liste_codes_INSEE_communes)
        emprise_communale.first.insert(commune);

    /* Coordonnées XY du rectangle d'emprise */
    FichierCsv *fc = new FichierCsv("/databank/cartographie/localisation/communes_XY_" + DEPARTEMENT);
    fc->Lire();

    if (DEPARTEMENT == "57")
        emprise_communale.second << 1032594 << 6937459 << 913391 << 6843314;
    else if (DEPARTEMENT == "54")
        emprise_communale.second << 1001188 << 6940350 << 879195 << 6812182;

    for (int i(1); i < fc->matrix.size(); ++i)
        foreach(QString code, emprise_communale.first)
            if (code == fc->matrix[i][0]){
                if (fc->matrix[i][1].toDouble() < emprise_communale.second[0])
                    emprise_communale.second[0] = fc->matrix[i][1].toDouble();
                if (fc->matrix[i][2].toDouble() < emprise_communale.second[1])
                    emprise_communale.second[1] = fc->matrix[i][2].toDouble();
                if (fc->matrix[i][3].toDouble() > emprise_communale.second[2])
                    emprise_communale.second[2] = fc->matrix[i][3].toDouble();
                if (fc->matrix[i][4].toDouble() > emprise_communale.second[3])
                    emprise_communale.second[3] = fc->matrix[i][4].toDouble();
            }
}
