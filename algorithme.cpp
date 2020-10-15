#include "algorithme.h"

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

    FichierCsv *f2 = new FichierCsv("/databank/MAJIC_2018/nonBati");
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
        FichierCsv *f3 = new FichierCsv("/databank/MAJIC_2018/parcelleBatie_" + code_INSEE);
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

    /* Correctifs et synthèses de l'occupation du sol au temps t
       --------------------------------------------------------- */
    foncier.artificialise += voirie_publique;
    foncier.enaf = (surface_ign - foncier.artificialise) /HA;
    foncier.non_bati = (foncier.artificialise - foncier.habitat - foncier.non_residentiel - foncier.dependance - foncier.a_batir) / HA ;
    foncier.artificialise /= HA; foncier.habitat /= HA; foncier.non_residentiel /= HA; foncier.dependance /= HA; foncier.a_batir /= HA;

    /* Correctifs du conteneur de l'analyse historique
       ----------------------------------------------- */
    for (int annee(p->Annee_debut()); annee <= p->Annee_fin(); ++annee){
        usages_par_annee[annee].habitat_individuel += 0;
        usages_par_annee[annee].habitat_collectif += 0;
        usages_par_annee[annee].non_residentiel += 0;
    }
}

QVector<QPointF> Algorithme::Habitat_ind(void)
{
    QVector<QPointF> xy;

    Usage cumule, annuel; double enaf_init(0.0);

    for (QMap<int, Usage>::const_iterator it = usages_par_annee.cbegin(); it != usages_par_annee.cend(); ++it){

        cumule.habitat_individuel += it.value().habitat_individuel;
        cumule.artificialise = (cumule.habitat_individuel + cumule.habitat_collectif + cumule.non_residentiel) / ((foncier.habitat + foncier.non_residentiel) / foncier.artificialise);
        cumule.enaf = surface_ign / HA - cumule.artificialise;

        if (it.key() >= periode->Annee_debut())
            enaf_init = cumule.enaf;

        if (it.key() >= periode->Annee_debut()
                && (it.key() - periode->Annee_debut()) % periode->Pas_de_temps() == 0
                && it.key() < periode->Annee_fin()){
            QPointF p;
            p.rx() = it.key();
            p.ry() = cumule.habitat_individuel;
            xy.append(p);
        }
    }

    return xy;
}
