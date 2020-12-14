#include "algorithme.h"

/**
 * \brief      Constructeur de la classe
 * \details    Le constructeur contient la totalité de l'algorithme de traitement des données pour le calcul
 *             de l'occupatio du sol au moment du dernier millésime des données \link Foncier \endlink,
 *             et l'évolution de ses usages durant la période définie \link Usages_par_annee \endlink
 * \param      t  Le territoire d'étude \link Territoire \endlink
 * \param      p  La période d'analyse \link Periode \endlink
 */
Algorithme::Algorithme(const Territoire * t, const Periode * p) : periode(p)
{
    /* Surface totale du ban communal (IGN BD Topo)
       -------------------------------------------- */
    FichierCsv *f1 = new FichierCsv("/databank/territoire/surface_IGN");
    f1->Lire();

    for (int i(1); i < f1->matrix.size(); ++i)
        if (t->Liste_codes_INSEE_communes().contains(f1->matrix[i][0]))
            surface_ign += f1->matrix[i][1].toDouble();

    delete f1;

    /* Gestion des surfaces non bâties
       ------------------------------- */
    double parcelle_cadastre(0.0), eau_cadastre(0.0), eau_BDTopo(0.0);

    FichierCsv *f2 = new FichierCsv("/databank/MAJIC_" + QString::number(periode->Millesime()) + "/nonBati_" + t->Departement());
    f2->Lire();

    for (int i(1); i < f2->matrix.size(); ++i)
        if (t->Liste_codes_INSEE_communes().contains(f2->matrix[i][0])){
            parcelle_cadastre += f2->matrix[i][1].toDouble();
            foncier.artificialise += f2->matrix[i][2].toDouble();
            eau_cadastre += f2->matrix[i][6].toDouble();
            foncier.a_batir += f2->matrix[i][7].toDouble();
            eau_BDTopo += f2->matrix[i][9].toDouble();
        }

    delete  f2;

    /* Gestion des parcelles bâties
       ---------------------------- */
    foreach (QString const& code_INSEE, t->Liste_codes_INSEE_communes()) {
        FichierCsv *f3 = new FichierCsv("/databank/MAJIC_" + QString::number(periode->Millesime()) + "/parcelleBatie_" + code_INSEE);
        f3->Lire();

        for (int i(1); i < f3->matrix.size(); ++i){
            parcelle_cadastre += f3->matrix[i][1].toDouble();
            foncier.artificialise += f3->matrix[i][3].toDouble();
            eau_cadastre += f3->matrix[i][7].toDouble();
            int annee_construction = f3->matrix[i][2].toInt();

            if (f3->matrix[i][13].toDouble() > 0)
                foncier.habitat += f3->matrix[i][3].toDouble() * f3->matrix[i][13].toDouble() / (f3->matrix[i][13].toDouble() + f3->matrix[i][14].toDouble());
            if (f3->matrix[i][11].toDouble() > 0)
                usages_par_annee[annee_construction].habitat_individuel += f3->matrix[i][3].toDouble() * f3->matrix[i][11].toDouble() / (f3->matrix[i][13].toDouble() + f3->matrix[i][14].toDouble()) / HA;
            if (f3->matrix[i][12].toDouble() > 0)
                usages_par_annee[annee_construction].habitat_collectif += f3->matrix[i][3].toDouble() * f3->matrix[i][12].toDouble() / (f3->matrix[i][13].toDouble() + f3->matrix[i][14].toDouble()) / HA;
            if (f3->matrix[i][14].toDouble() > 0) {
                foncier.non_residentiel += f3->matrix[i][3].toDouble() * f3->matrix[i][14].toDouble() / (f3->matrix[i][13].toDouble() + f3->matrix[i][14].toDouble());
                usages_par_annee[annee_construction].non_residentiel += f3->matrix[i][3].toDouble() * f3->matrix[i][14].toDouble() / (f3->matrix[i][13].toDouble() + f3->matrix[i][14].toDouble()) / HA;
            }
            if (f3->matrix[i][15].toDouble() > 0 && f3->matrix[i][13].toDouble() == 0.0 && f3->matrix[i][14].toDouble() == 0.0)
                foncier.dependance += f3->matrix[i][3].toDouble();
            if (f3->matrix[i][9].toDouble() == 0.0)
                foncier.a_batir += f3->matrix[i][8].toDouble();
        }

        delete f3;
    }

    /* Ajustement des cas limites
       -------------------------- */
    double non_cadastre(surface_ign - parcelle_cadastre);
    if (non_cadastre < 0)
        non_cadastre = 0;
    double eau_domaniale(eau_BDTopo - eau_cadastre);
    if (eau_domaniale < 0)
        eau_domaniale = 0;
    if (eau_cadastre > eau_BDTopo)
        eau_BDTopo = eau_cadastre;
    double voirie_publique(non_cadastre - eau_domaniale);
    if (voirie_publique < 0)
        voirie_publique = 0;

    /* Correctifs et synthèses de l'occupation du sol au moment du millésime de données
       -------------------------------------------------------------------------------- */
    foncier.artificialise += voirie_publique;
    foncier.enaf = (surface_ign - foncier.artificialise) /HA;
    foncier.non_bati = (foncier.artificialise - foncier.habitat - foncier.non_residentiel - foncier.dependance - foncier.a_batir) / HA ;
    foncier.artificialise /= HA; foncier.habitat /= HA; foncier.non_residentiel /= HA; foncier.dependance /= HA; foncier.a_batir /= HA;

    /* Correctifs du conteneur de l'évolution des usages du sol au cours de la période définie
       --------------------------------------------------------------------------------------- */
    for (int annee(p->Annee_debut()); annee <= p->Annee_fin(); ++annee){
        usages_par_annee[annee].habitat_individuel += 0;
        usages_par_annee[annee].habitat_collectif += 0;
        usages_par_annee[annee].non_residentiel += 0;
    }
}

/**
 * \brief      Mesure l'évolution cumulée du bâti (selon son usage)
 * \details    Calcule les coordonnées XY, où X = année et Y = nombre de m2 affectés à cet usage
 *             depuis le début de la période. \n Ces coordonnées sont destinées à tracer la courbe d'un graphique.
 * \param      arg \link Usage \endlink du sol : <em> habitat_individuel, habitat_collectif ou non_residentiel </em>
 * \return     Tableau de coordonnées XY
 */
QVector<QPointF> Algorithme::Bati_cumule(double Usage:: *arg)
{
    QVector<QPointF> xy;

    double cumule(0.0);

    for (QMap<int, Usage>::const_iterator it = usages_par_annee.cbegin(); it != usages_par_annee.cend(); ++it){

        cumule += it.value().*arg;

        if (it.key() >= periode->Annee_debut()
                && (it.key() - periode->Annee_debut()) % periode->Pas_de_temps() == 0
                && it.key() <= periode->Annee_fin()){
            QPointF p;
            p.rx() = it.key();
            p.ry() = cumule;
            xy.append(p);
        }
    }
    return xy;
}

/**
 * \brief      Mesure l'évolution courante du bâti (selon son usage)
 * \details    Calcule les coordonnées XY, où X = année et Y = nombre de m2 affectés à cet usage par année. \n
 *             Ces coordonnées sont destinées à tracer la courbe d'un graphique.
 * \param      arg \link Usage \endlink du sol : <em> habitat_individuel, habitat_collectif ou non_residentiel </em>
 * \return     Tableau de coordonnées XY
 */
QVector<QPointF> Algorithme::Bati_courant(double Usage:: *arg)
{
    QVector<QPointF> xy;

    double courant(0.0);

    for (QMap<int, Usage>::const_iterator it = usages_par_annee.cbegin(); it != usages_par_annee.cend(); ++it){

        if (it.key() > periode->Annee_debut())
            courant += it.value().*arg;
        else
            courant = it.value().*arg;

        if (it.key() >= periode->Annee_debut()
                && (it.key() - periode->Annee_debut()) % periode->Pas_de_temps() == 0
                && it.key() <= periode->Annee_fin()){
            QPointF p;
            p.rx() = it.key();
            p.ry() = courant / periode->Pas_de_temps();
            xy.append(p);

            courant = 0;
        }
    }
    return xy;
}

/**
 * \brief      Mesure l'évolution cumulée de la consommation foncière
 * \details    Calcule les coordonnées XY, où X = année et Y = nombre de m2 artificialisés depuis le début de la période.\n
 *             Ces coordonnées sont destinées à tracer la courbe d'un graphique.
 * \param      void
 * \return     Tableau de coordonnées XY (Lambert 93 - EPSG 2154)
 */
QVector<QPointF> Algorithme::Conso_Foncier_Cumul(void)
{
    QVector<QPointF> xy;

    Usage cumule; double enaf_init(0.0);

    for (QMap<int, Usage>::const_iterator it = usages_par_annee.cbegin(); it != usages_par_annee.cend(); ++it){

        cumule.habitat_individuel += it.value().habitat_individuel;
        cumule.habitat_collectif += it.value().habitat_collectif;
        cumule.non_residentiel += it.value().non_residentiel;

        cumule.artificialise = (cumule.habitat_individuel + cumule.habitat_collectif + cumule.non_residentiel) / ((foncier.habitat + foncier.non_residentiel) / foncier.artificialise);
        cumule.enaf = surface_ign / HA - cumule.artificialise;

        if (it.key() == periode->Annee_debut())
            enaf_init = cumule.enaf;

        if (it.key() >= periode->Annee_debut()
                && (it.key() - periode->Annee_debut()) % periode->Pas_de_temps() == 0
                && it.key() <= periode->Annee_fin()){
            QPointF p;
            p.rx() = it.key();
            p.ry() = enaf_init - cumule.enaf;
            xy.append(p);
        }
    }

    return xy;
}

/**
 * \brief      Mesure l'évolution courante de la consommation foncière
 * \details    Calcule les coordonnées XY, où X = année et Y = nombre de m2 artificialisés par année.\n
 *             Ces coordonnées sont destinées à tracer la courbe d'un graphique.
 * \param      xy_cumul Coordonnées XY de la consommation foncière cumulée \link Conso_Foncier_Cumul \endlink
 * \return     Tableau de coordonnées XY
 */
QVector<QPointF> Algorithme::Conso_Foncier_Courant(QVector<QPointF> xy_cumul)
{
    QVector<QPointF> xy;

    for (int i(1); i < xy_cumul.size(); ++i){
        QPointF p;
        p.rx() = xy_cumul[i].rx();
        p.ry() = (xy_cumul[i].ry() - xy_cumul[i-1].ry()) / periode->Pas_de_temps();
        xy.append(p);
    }

    return xy;
}
