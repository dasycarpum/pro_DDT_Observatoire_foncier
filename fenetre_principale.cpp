#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Granularités territoriales : commune, EPCI, SCoT, département */
    grp_granularite = new QButtonGroup(ui->groupBox_territoire);
    grp_granularite->addButton(ui->radioButton_commune);
    grp_granularite->addButton(ui->radioButton_epci);
    grp_granularite->addButton(ui->radioButton_scot);
    grp_granularite->addButton(ui->radioButton_departement);
    connect(grp_granularite, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(Gestion_granularite_territoire(QAbstractButton *)));

    /* Initialisation du territoire d'étude */
    ui->radioButton_commune->setChecked(true);
    Gestion_granularite_territoire(ui->radioButton_commune);

    /* Initialisation de la période d'observation */
    Periode *p = new Periode();
    p->Dernier_millesime_majic();
    p->Ajustement_automatique_annee_debut();
    ui->spinBox_an_debut->setValue(p->Annee_debut());
    ui->spinBox_an_fin->setValue(p->Annee_fin());
    ui->spinBox_an_pas->setValue(p->Pas_de_temps());

    /* Validation des saisies utilisateur */
    connect(ui->pushButton_validation, SIGNAL(clicked(bool)), this, SLOT(Validation_des_saisies(bool)));

}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
    delete grp_granularite;
    delete territoire;
}

void FenetrePrincipale::Gestion_granularite_territoire(QAbstractButton * button)
{
    ui->comboBox_geographie->clear();
    QStringList liste_geographies = Territoire::Liste_libelles_geographies(button->text());
    ui->comboBox_geographie->addItems(liste_geographies);
}

void FenetrePrincipale::Validation_des_saisies(bool)
{
    /* Territoire d'étude */
    QPair<QString, QString> geographie;
    geographie.second = ui->comboBox_geographie->currentText();
    territoire = new Territoire(grp_granularite->button(grp_granularite->checkedId())->text(), geographie);

    /* Période d'observation */
    periode = new Periode();
    periode->Dernier_millesime_majic();
    periode->Annee_debut(ui->spinBox_an_debut->value());
    periode->Annee_fin(ui->spinBox_an_fin->value());
    periode->Pas_de_temps(ui->spinBox_an_pas->value());

    /* Calcul des occupations du sol par année */
    Algorithme *algorithme = new Algorithme(territoire, periode);

    /* Affichage des tableaux de résultats */
    Affichage_tableau_occupation(algorithme);
    Affichage_tableau_evolution(algorithme);

    /* Affichage des graphiques de résultats */
    if (!ui->horizontalLayout_bati_cumul->isEmpty())
        ui->horizontalLayout_bati_cumul->removeWidget(graph_bati_cumul);
    Affichage_graphique_bati_cumul(algorithme);

}

void FenetrePrincipale::Affichage_tableau_occupation(const Algorithme * algo)
{
    /* Rafraichissement de la zone d'édition */
    ui->textBrowser_resultat->clear();

    /* Titre = territoire d'étude et millésime des données */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=600 CELLSPACING=3 ALIGN=center>"
                                                "<TR>"
                                                    "<TD align=center><h2>%1 : %2</h2><h3>Données foncières <small>(MAJIC - DGFiP)</small> au 1<SUP>er</SUP> janvier %3</h2></TD>"
                                                "</TR>"
                                             "</TABLE><br>")
                                                .arg(territoire->Granularite())
                                                .arg(territoire->Geographie().second)
                                                .arg(periode->Millesime()));

    /* Occupation du sol au dernier millésime de données */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=600 CELLSPACING=3 align=center>"
                                                "<TR>"
                                                    "<TD align=center colspan=2><h2>Occupation du sol</h2></TD>"
                                                "</TR>"
                                                "<TR>"
                                                    "<TD WIDTH=360 align=center><h3><br>Surface artificialisée : %L1 ha</h3></TD>"
                                                    "<TD align=left><b><br>&nbsp;Bâti : %L2 ha</b>"
                                                                   "<BLOCKQUOTE>Habitat : %L3 ha</BLOCKQUOTE>"
                                                                   "<BLOCKQUOTE>Non résidentiel : %L4 ha</BLOCKQUOTE>"
                                                                   "<BLOCKQUOTE>Dépendance : %L5 ha</BLOCKQUOTE>"
                                                                   "<b>&nbsp;Non bâti : %L6 ha</b><br><br>"
                                                                   "<b>&nbsp;A bâtir : %L7 ha</b><br></TD>"
                                                "</TR>"
                                                "<TR>"
                                                    "<TD align=center><h3><br>Espace agricole naturel et forestier : %L8 ha<br></h3></TD>"
                                                "</TR>"
                                             "</TABLE><br>")
                                                .arg(algo->Foncier().artificialise, 0, 'f', 2)
                                                .arg(algo->Foncier().habitat + algo->Foncier().non_residentiel + algo->Foncier().dependance, 0, 'f', 2)
                                                .arg(algo->Foncier().habitat, 0, 'f', 2)
                                                .arg(algo->Foncier().non_residentiel, 0, 'f', 2)
                                                .arg(algo->Foncier().dependance, 0, 'f', 2)
                                                .arg(algo->Foncier().non_bati, 0, 'f', 2)
                                                .arg(algo->Foncier().a_batir, 0, 'f', 2)
                                                .arg(algo->Foncier().enaf, 0, 'f', 2));
}

void FenetrePrincipale::Affichage_tableau_evolution(const Algorithme * algo)
{
    /* Titre et en-têtes de colonnes */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=600 CELLSPACING=3 ALIGN=center>"
                                                 "<TR>"
                                                     "<TD align=center colspan=7><h2>Evolution de l'occupation du sol entre %1 et %2</h2></TD>"
                                                 "</TR>"
                                                 "<TR>"
                                                     "<TD WIDTH=65 align=center><b>Année</b></TD>"
                                                     "<TD WIDTH=80 align=center><b>Habitat individuel</b></TD>"
                                                     "<TD WIDTH=77 align=center><b>Habitat collectif</b></TD>"
                                                     "<TD WIDTH=80 align=center><b>Non résidentiel</b></TD>"
                                                     "<TD WIDTH=80 align=center><b>Total artificialisé</b></TD>"
                                                     "<TD WIDTH=80 align=center><b>Espace nat., agri. et forestier</b></TD>"
                                                     "<TD align=center><b>Consommation foncière cumulée</b></TD>"
                                                 "</TR>"
                                             "</TABLE>")
                                                .arg(periode->Annee_debut())
                                                .arg(periode->Annee_fin()));

    Usage surf_cumul; double enaf_init(0.0); int i(0);

    for (QMap<int, Usage>::const_iterator it = algo->Usages_par_annee().cbegin(); it != algo->Usages_par_annee().cend(); ++it){
        surf_cumul.habitat_individuel += it.value().habitat_individuel;
        surf_cumul.habitat_collectif += it.value().habitat_collectif;
        surf_cumul.non_residentiel += it.value().non_residentiel;

        if (it.key() >= periode->Annee_debut()
                && (it.key() - periode->Annee_debut()) % periode->Pas_de_temps() == 0
                && it.key() <= periode->Annee_fin()){
            surf_cumul.artificialise = (surf_cumul.habitat_individuel + surf_cumul.habitat_collectif + surf_cumul.non_residentiel) / ((algo->Foncier().habitat + algo->Foncier().non_residentiel) / algo->Foncier().artificialise);
            surf_cumul.enaf = algo->Surface_ign() / HA - surf_cumul.artificialise;
            if (i == 0)
                enaf_init = surf_cumul.enaf;

            ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=600 CELLSPACING=3 ALIGN=center>"
                                                         "<TR>"
                                                             "<TD WIDTH=65 align=center><b>%1</b></TD>"
                                                             "<TD WIDTH=80 align=center>%L2 ha</TD>"
                                                             "<TD WIDTH=77 align=center>%L3 ha</TD>"
                                                             "<TD WIDTH=80 align=center>%L4 ha</TD>"
                                                             "<TD WIDTH=80 align=center>%L5 ha</TD>"
                                                             "<TD WIDTH=80 align=center>%L6 ha</TD>"
                                                             "<TD align=center>%L7 ha - %L8 %</TD>"
                                                         "</TR>"
                                                     "</TABLE>")
                                                         .arg(QString::number(it.key()))
                                                         .arg(surf_cumul.habitat_individuel, 0, 'f', 1)
                                                         .arg(surf_cumul.habitat_collectif, 0, 'f', 1)
                                                         .arg(surf_cumul.non_residentiel, 0, 'f', 1)
                                                         .arg(surf_cumul.artificialise, 0, 'f', 1)
                                                         .arg(surf_cumul.enaf, 0, 'f', 1)
                                                         .arg(enaf_init - surf_cumul.enaf, 0, 'f', 1)
                                                         .arg((enaf_init - surf_cumul.enaf) * 100 / enaf_init, 0, 'f', 1));

            ++i;
        }
    }
}

class EchelleTxt : public QwtScaleDraw
{
    virtual QwtText label (double value) const override
    {
        return QwtText(QString::number(value, 'f', 0));
    }
};

void FenetrePrincipale::Affichage_graphique_bati_cumul(Algorithme * algo)
{
    /* Graphique */
    graph_bati_cumul = new QwtPlot(this);
    graph_bati_cumul->setTitle("Surfaces cumulées du bâti");
    graph_bati_cumul->setAxisTitle(QwtPlot::xBottom, "Année");
    graph_bati_cumul->setAxisTitle(QwtPlot::yLeft, "Surface en hectare");
    graph_bati_cumul->setCanvasBackground(QBrush(QColor("#D9CCB0")));

    /* Format de l'échelle */
    EchelleTxt * x = new EchelleTxt();
    x->setLabelRotation(-45);
    graph_bati_cumul->setAxisScaleDraw(QwtPlot::xBottom, x);

    /* Grille */
    QwtPlotGrid *grille = new QwtPlotGrid();
    grille->setPen(QPen(Qt::darkGray, 0 , Qt::DotLine));
    grille->attach(graph_bati_cumul);

    /* Courbes */
    QwtPlotCurve *curve1 = new QwtPlotCurve( "Habitat individuel" );
    curve1->setSamples(algo->Bati_cumule(&Usage::habitat_individuel));
    curve1->setCurveAttribute(QwtPlotCurve::Fitted);
    curve1->setPen(QColor(Qt::red), 2);
    curve1->attach(graph_bati_cumul);

    QwtPlotCurve *curve2 = new QwtPlotCurve( "Habitat collectif" );
    curve2->setSamples(algo->Bati_cumule(&Usage::habitat_collectif));
    curve2->setCurveAttribute(QwtPlotCurve::Fitted);
    curve2->setPen(QColor(Qt::darkRed), 2);
    curve2->attach(graph_bati_cumul);

    QwtPlotCurve *curve3 = new QwtPlotCurve( "Non résidentiel" );
    curve3->setSamples(algo->Bati_cumule(&Usage::non_residentiel));
    curve3->setCurveAttribute(QwtPlotCurve::Fitted);
    curve3->setPen(QColor(Qt::darkMagenta), 2);
    curve3->attach(graph_bati_cumul);

    /* Légende */
    QwtLegend *legend = new QwtLegend();
    curve1->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    curve2->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    curve3->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    graph_bati_cumul->insertLegend(legend, QwtPlot::RightLegend);

    /* Insertion du graphique */
    ui->horizontalLayout_bati_cumul->insertWidget(0, graph_bati_cumul);
    graph_bati_cumul->replot();
}
