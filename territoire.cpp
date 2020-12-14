#include "territoire.h"

/**
 * \brief      Constructeur de la classe
 * \param      dep  Numéro du département \a "54", "57", ...
 * \param      gra  Libellé de la granularité \a "Commune", "EPCI", "SCoT" ou "Département"
 * \param      geo  Code et libellé de la géographie \a p.ex. ("57463", "Metz")
 */
Territoire::Territoire(QString const& dep, QString const& gra, QPair<QString, QString> const& geo) : departement(dep), granularite(gra), geographie(geo)
{
    if (geographie.first.isEmpty()){

        /* Recherche du n° de champ du libellé géographique, en fonction de la granularité */
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

        /* Constitution de la liste des codes INSEE communaux du territoire */
        FichierCsv *cl = new FichierCsv("/databank/territoire/code_libelle");
        cl->Lire();

        for (int i(1); i < cl->matrix.size(); ++i)
            if (cl->matrix[i][champ_cod_dep] == departement && cl->matrix[i][champ_lib_geo] == geographie.second)
                liste_codes_INSEE_communes.append(cl->matrix[i][champ_cod_com]);

        delete cl;   
    }
}

/**
 * \brief      Liste les géographies d'une granularité particulière
 * \details    Cette fonction est utilisée pour afficher les géographies d'une granularité dans une QComboBox \a ui->comboBox_geographie
 *
 * \param      dep  Numéro du département \a "54", "57", ...
 * \param      gra  Libellé de la granularité \a "Commune", "EPCI", "SCoT" ou "Département"
 * \return     Liste de libellés géographiques d'une granularité
 */
QStringList Territoire::Liste_libelles_geographies(QString const& dep, QString const& gra)
{
    QMap<QString, bool> liste_libelle;

    /* Recherche du n° de champ des libellés géographiques, en fonction de la granularité */
    int champ_lib_geo(0), champ_cod_dep(0);

    FichierCsv *g = new FichierCsv("/databank/territoire/granularite");
    g->Lire();

    for (int i(1); i < g->matrix.size(); ++i){
        if (g->matrix[i][0] == gra)
            champ_lib_geo = g->matrix[i][2].toInt();
        if (g->matrix[i][0] == "Département")
            champ_cod_dep =  g->matrix[i][1].toInt();
    }
    delete g;

    /* Constitution de la liste des libellés géographiques, en fonction de la granularité */
    FichierCsv *cl = new FichierCsv("/databank/territoire/code_libelle");
    cl->Lire();

    for (int i(1); i < cl->matrix.size(); ++i)
        if (cl->matrix[i][champ_cod_dep] == dep)
            liste_libelle[cl->matrix[i][champ_lib_geo]] = true;

    delete cl;

    return liste_libelle.keys();
}

/**
 * \brief      Calcule les coordonnées du rectangle contenant l'emprise géographique du territoire d'étude
 * \details    Fournit les 2 coordonnées Lambert 93 (EPSG 2154) de la diagonale du rectangle : X_min, Y_min, X_max, Y_max
 *             à partir d'un fichier des coordonnées communales \a /databank/cartographie/localisation/communes_XY_dep.csv
 * \param      void
 * \return     void
 */
void Territoire::Evaluation_emprise_communale(void)
{
    /* Constituion de la liste unique des codes INSEE communaux du territoire */
    foreach (QString const& commune, liste_codes_INSEE_communes)
        emprise_communale.first.insert(commune);

    /* Calcul des coordonnées XY du rectangle (diagonale) de l'emprise du territoire */
    FichierCsv *fc = new FichierCsv("/databank/cartographie/localisation/communes_XY_" + departement);
    fc->Lire();

    if (departement == "57")
        emprise_communale.second << 1032594 << 6937459 << 913391 << 6843314;
    else if (departement == "54")
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
