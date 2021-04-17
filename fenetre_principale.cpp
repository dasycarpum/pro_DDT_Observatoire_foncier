#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

/**
 * \brief      Constructeur de la classe
 */
FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Style */
    ui->groupBox_periode->setObjectName("box");
    ui->groupBox_periode->setStyleSheet("QGroupBox#box { font-weight:bold }");
    ui->groupBox_territoire->setObjectName("box");
    ui->groupBox_territoire->setStyleSheet("QGroupBox#box { font-weight:bold }");
    ui->groupBox_resultat->setObjectName("box");
    ui->groupBox_resultat->setStyleSheet("QGroupBox#box { font-weight:bold }");

    QPalette palette; palette.setColor(QPalette::Button, QColor(177, 171, 160));
    ui->pushButton_validation->setPalette(palette);
    ui->pushButton_cartographie->setPalette(palette);

    /* Départements : 54 ou 57 */
    grp_departement = new QButtonGroup(ui->groupBox_departement);
    grp_departement->setExclusive(true);
    grp_departement->addButton(ui->checkBox_54);
    grp_departement->addButton(ui->checkBox_57);
    connect(grp_departement, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(Gestion_granularite_territoire(QAbstractButton *)));

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
    connect(ui->pushButton_cartographie, SIGNAL(clicked(bool)), this, SLOT(Cartographie(bool)));

    /* Menus */
    connect(ui->action_imprimer_pdf, SIGNAL(triggered()), this, SLOT(Menu_imprimer_pdf()));
    connect(ui->action_imprimer_doc, SIGNAL(triggered()), this, SLOT(Menu_imprimer_doc()));
    connect(ui->action_exporter_jpeg, SIGNAL(triggered()), this, SLOT(Menu_exporter_jpeg()));
    connect(ui->action_manuel_utilisation, SIGNAL(triggered()), this, SLOT(Menu_aide()));
    connect(ui->action_methodologie, SIGNAL(triggered()), this, SLOT(Menu_aide()));
}

/**
 * \brief      Destructeur de la classe
 */
FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
    delete grp_granularite;
    delete territoire;
}

/**
 * \brief      SLOT - Liste les géographies d'une granularité particulière
 * \details    Cette fonction est connectée à 2 groupes de boutons (<em> grp_departement & grp_granularite </em>) et
 *             est utilisée pour afficher les géographies d'une granularité dans une QComboBox (\a ui->comboBox_geographie)
 *             lorsque un QRadioButton est cliqué; si le bouton cliqué fait partie du \a grp_departement ,
 *             cela réinitialise la granularité à la commune du département sélectionné par l'utilisateur.
 * \param      button  QCheckBox du département ou QRadioButton de la granularité;
 * \return     void
 */
void FenetrePrincipale::Gestion_granularite_territoire(QAbstractButton * button)
{
    if (button->group() == grp_departement){
        ui->radioButton_commune->setChecked(true);
        Gestion_granularite_territoire(ui->radioButton_commune);
    }
    else {
        ui->comboBox_geographie->clear();
        QStringList liste_geographies = Territoire::Liste_libelles_geographies(grp_departement->button(grp_departement->checkedId())->text().left(2),
                                                                           button->text());
        ui->comboBox_geographie->addItems(liste_geographies);
    }
}

/**
 * \brief      SLOT - Validation de la requête
 * \details    Cette fonction est connectée à un bouton (\a ui->pushButton_validation ); elle traite la requête utilisateur
 *             selon le territoire et la période choisis, pour afficher tableaux et graphiques de résultats
 * \param      bool
 * \return     void
 */
void FenetrePrincipale::Validation_des_saisies(bool)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    /* Territoire d'étude */
    QPair<QString, QString> geographie;
    geographie.second = ui->comboBox_geographie->currentText();
    territoire = new Territoire(grp_departement->button(grp_departement->checkedId())->text().left(2),
                                grp_granularite->button(grp_granularite->checkedId())->text(),
                                geographie);

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
    Nettoyage_graphique(ui->horizontalLayout_bati_cumul);
    Affichage_graphique_bati_cumul(algorithme);
    Nettoyage_graphique(ui->horizontalLayout_bati_courant);
    Affichage_graphique_bati_courant(algorithme);
    Nettoyage_graphique(ui->horizontalLayout_conso_foncier);
    Affichage_graphique_conso_fonciere(algorithme);

    /* Cartographie */
    ui->pushButton_cartographie->setEnabled(true);

    QApplication::restoreOverrideCursor();
}

/**
 * \brief      Affichage du tableau d'occupations du sol
 * \details    Cette fonction affiche dans un \a QTextBrowser (\a ui->textBrowser_resultat ), un titre (détails du territoire
 *             et millésime de données) et un tableau répertoriant les usages du sol les plus récents.
 * \param      algo Classe \link Algorithme \endlink, qui donne accès à son accesseur \link Algorithme::Foncier \endlink de type \link Usage \endlink
 * \return     void
 */
void FenetrePrincipale::Affichage_tableau_occupation(const Algorithme * algo)
{
    /* Rafraichissement de la zone d'édition */
    ui->textBrowser_resultat->clear();

    /* Titre = territoire d'étude et millésime des données */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=640 CELLSPACING=3 ALIGN=center>"
                                                "<TR>"
                                                    "<TD align=center><h2>%1 : %2</h2><h3>Données foncières <small>(MAJIC - DGFiP)</small> au 1<SUP>er</SUP> janvier %3</h2></TD>"
                                                "</TR>"
                                             "</TABLE><br>")
                                                .arg(territoire->Granularite())
                                                .arg(territoire->Geographie().second)
                                                .arg(periode->Millesime()));

    /* Occupation du sol au dernier millésime de données */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=640 CELLSPACING=3 align=center>"
                                                "<TR>"
                                                    "<TD align=center colspan=2><h2>Occupation du sol</h2></TD>"
                                                "</TR>"
                                                "<TR>"
                                                    "<TD WIDTH=410 align=center><h3><br>Surface artificialisée : %L1 ha</h3></TD>"
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

/**
 * \brief      Affichage du tableau de l'évolution des usages du sol
 * \details    Cette fonction affiche dans un \a QTextBrowser (\a ui->textBrowser_resultat ), un tableau répertoriant
 *             pour la période définie, les usages cumulés du sol par pas de temps.
 * \param      algo Classe \link Algorithme \endlink, qui donne accès à son accesseur \link Algorithme::Usages_par_annee \endlink de type \a QMap<int, \link Usage \endlink >
 * \return     void
 */
void FenetrePrincipale::Affichage_tableau_evolution(const Algorithme * algo)
{
    /* Titre et en-têtes de colonnes */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=640 CELLSPACING=3 ALIGN=center>"
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

            ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=640 CELLSPACING=3 ALIGN=center>"
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

/**
 * \brief   Conversion d'un axe graphique numérique en texte
 * \details Cette classe permet l'écriture des étiquettes de l'abscisse ou de l'ordonnée d'un graphique (\a QwtPlot )
 *          au format texte, en lui transmettant une valeur de type \a double
 */
class EchelleTxt : public QwtScaleDraw
{
    virtual QwtText label (double value) const override
    {
        return QwtText(QString::number(value, 'f', 0));
    }
};

/**
 * \brief      Suppression de tous les widgets d'un \a QLayout
 * \details    Cette fonction liste tous les widgets du layout transmis en paramètres et les supprime.
 *             Utilisé pour rafraîchir les graphiques de type \a QwtPlot .
 * \param      layout Gestionnaire de géométrie \a QLayout , quelle que soit sa classe héritée.
 * \return     void
 */
void FenetrePrincipale::Nettoyage_graphique(QLayout * layout)
{
    QLayoutItem* child;

    while (layout->count() != 0){
        child = layout->takeAt(0);

        if (child->layout() != nullptr)
            layout->removeItem(child);
        else if (child->widget() != nullptr)
            delete child->widget();

        delete child;
    }
}

/**
 * \brief      Affichage du graphique présentant l'évolution cumulée des surfaces bâties
 * \details    Cette fonction affiche dans un \a QwtPlot, un graphique représentant les 3 courbes des surfaces bâties :
 *             habitat individuel, habitat collectif et le bâti non résidentiel, couvrant la période spécifiée.
 * \param      algo Classe \link Algorithme \endlink, qui donne accès à sa fonction \link Algorithme::Bati_cumule \endlink de type \a QVector<QPointF>
 * \return     void
 */
void FenetrePrincipale::Affichage_graphique_bati_cumul(Algorithme * algo)
{
    /* Graphique : présentation */
    graph_bati_cumul = new QwtPlot(this);
    graph_bati_cumul->setTitle("Surfaces cumulées du bâti");
    graph_bati_cumul->setAxisTitle(QwtPlot::xBottom, "Année");
    graph_bati_cumul->setAxisTitle(QwtPlot::yLeft, "hectare");
    graph_bati_cumul->setCanvasBackground(QBrush(QColor("#d7d0c3")));

    /* Format de l'échelle */
    EchelleTxt * x = new EchelleTxt();
    x->setLabelRotation(-45);
    graph_bati_cumul->setAxisScaleDraw(QwtPlot::xBottom, x);

    /* Grille */
    QwtPlotGrid *grille = new QwtPlotGrid();
    grille->setPen(QPen(Qt::darkGray, 0 , Qt::DotLine));
    grille->attach(graph_bati_cumul);

    /* Courbes */
    QVector<QString> titre = {"Habitat individuel", "Habitat collectif", "Non résidentiel"};
    QVector<double Usage:: *> usage = {&Usage::habitat_individuel, &Usage::habitat_collectif, &Usage::non_residentiel};
    QVector<QColor> couleur = {Qt::red, Qt::darkRed, Qt::darkMagenta};

    auto tuple = std::make_tuple(titre, usage, couleur);

    for (int i(0); i < titre.size(); ++i){
        QwtPlotCurve *curve = new QwtPlotCurve( std::get<0>(tuple).at(i) );
        curve->setSamples(algo->Bati_cumule( std::get<1>(tuple).at(i)) );
        curve->setCurveAttribute(QwtPlotCurve::Fitted);
        curve->setPen(QColor( std::get<2>(tuple).at(i) ), 2);
        curve->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        curve->attach(graph_bati_cumul);
    }

    /* Légende */
    QwtLegend *legend = new QwtLegend();
    graph_bati_cumul->insertLegend(legend, QwtPlot::RightLegend);

    /* Insertion du graphique */
    ui->horizontalLayout_bati_cumul->insertWidget(0, graph_bati_cumul);
    graph_bati_cumul->replot();
}

/**
 * \brief      Affichage du graphique présentant l'évolution courante des surfaces bâties
 * \details    Cette fonction affiche dans un \a QwtPlot, un graphique représentant les 3 courbes des surfaces bâties :
 *             habitat individuel, habitat collectif et le bâti non résidentiel, couvrant la période spécifiée.
 * \param      algo Classe \link Algorithme \endlink, qui donne accès à sa fonction \link Algorithme::Bati_courant \endlink de type \a QVector<QPointF>
 * \return     void
 */
void FenetrePrincipale::Affichage_graphique_bati_courant(Algorithme * algo)
{
    /* Graphique : présentation */
    graph_bati_courant = new QwtPlot(this);
    graph_bati_courant->setTitle("Surfaces bâties annuellement");
    graph_bati_courant->setAxisTitle(QwtPlot::xBottom, "Année");
    graph_bati_courant->setAxisTitle(QwtPlot::yLeft, "hectare/an");
    graph_bati_courant->setCanvasBackground(QBrush(QColor("#d7d0c3")));

    /* Format de l'échelle */
    EchelleTxt * x = new EchelleTxt();
    x->setLabelRotation(-45);
    graph_bati_courant->setAxisScaleDraw(QwtPlot::xBottom, x);

    /* Grille */
    QwtPlotGrid *grille = new QwtPlotGrid();
    grille->setPen(QPen(Qt::darkGray, 0 , Qt::DotLine));
    grille->attach(graph_bati_courant);

    /* Courbes */
    QVector<QString> titre = {"Habitat individuel", "Habitat collectif", "Non résidentiel"};
    QVector<double Usage:: *> usage = {&Usage::habitat_individuel, &Usage::habitat_collectif, &Usage::non_residentiel};
    QVector<QColor> couleur = {Qt::red, Qt::darkRed, Qt::darkMagenta};

    auto tuple = std::make_tuple(titre, usage, couleur);

    for (int i(0); i < titre.size(); ++i){
        QwtPlotCurve *curve = new QwtPlotCurve( std::get<0>(tuple).at(i) );
        curve->setSamples(algo->Bati_courant( std::get<1>(tuple).at(i)) );
        curve->setCurveAttribute(QwtPlotCurve::Fitted);
        curve->setPen(QColor( std::get<2>(tuple).at(i) ), 2);
        curve->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        curve->attach(graph_bati_courant);
    }

    /* Légende */
    QwtLegend *legend = new QwtLegend();
    graph_bati_courant->insertLegend(legend, QwtPlot::RightLegend);

    /* Insertion du graphique */
    ui->horizontalLayout_bati_courant->insertWidget(0, graph_bati_courant);
    graph_bati_courant->replot();
}

/**
 * \brief      Affichage du graphique présentant l'évolution cumulée et courante de l'artificialisation des sols
 * \details    Cette fonction affiche dans un \a QwtPlot, un graphique représentant les 2 courbes de l'artificialisation :
 *             consommation foncière cumulée et annuelle, couvrant la période spécifiée.
 * \param      algo Classe \link Algorithme \endlink, qui donne accès aux fonctions \link Algorithme::Conso_Foncier_Cumul \endlink
 *             et \link Algorithme::Conso_Foncier_Courant \endlink de type \a QVector<QPointF>
 * \return     void
 */
void FenetrePrincipale::Affichage_graphique_conso_fonciere(Algorithme * algo)
{
    /* Graphique : présentation */
    graph_conso_foncier = new QwtPlot(this);
    graph_conso_foncier->setTitle("Consommation foncière");
    graph_conso_foncier->enableAxis(QwtPlot::yRight);
    graph_conso_foncier->setAxisTitle(QwtPlot::xBottom, "Année");
    graph_conso_foncier->setAxisTitle(QwtPlot::yLeft, "hectare");
    graph_conso_foncier->setAxisTitle(QwtPlot::yRight, "hectare/an");
    graph_conso_foncier->setCanvasBackground(QBrush(QColor("#b1aba0")));

    /* Format de l'échelle */
    EchelleTxt * x = new EchelleTxt();
    x->setLabelRotation(-45);
    graph_conso_foncier->setAxisScaleDraw(QwtPlot::xBottom, x);

    /* Grille */
    QwtPlotGrid *grille = new QwtPlotGrid();
    grille->setPen(QPen(Qt::darkGray, 0 , Qt::DotLine));
    grille->attach(graph_conso_foncier);

    /* Courbes */
    QwtPlotCurve *curve_cumul = new QwtPlotCurve( "Cumulée" );
    QVector<QPointF> xy = algo->Conso_Foncier_Cumul();
    curve_cumul->setSamples(xy);
    curve_cumul->setCurveAttribute(QwtPlotCurve::Fitted);
    curve_cumul->setPen(QColor( Qt::darkGreen ), 2);
    curve_cumul->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    curve_cumul->attach(graph_conso_foncier);

    QwtPlotCurve *curve = new QwtPlotCurve( "Courante" );
    curve->setSamples(algo->Conso_Foncier_Courant(xy));
    curve->setYAxis(QwtPlot::yRight);
    curve->setCurveAttribute(QwtPlotCurve::Fitted);
    curve->setPen(QColor( Qt::green ), 2);
    curve->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    curve->attach(graph_conso_foncier);

    /* Légende */
    QwtLegend *legend = new QwtLegend();
    graph_conso_foncier->insertLegend(legend, QwtPlot::RightLegend);

    /* Insertion du graphique */
    ui->horizontalLayout_conso_foncier->insertWidget(0, graph_conso_foncier);
    graph_conso_foncier->replot();
}

/*! \file
 * \fn      void Customisation_projet_QGiS(const Territoire * territoire)
 * \brief   Customisation du projet de cartographie de l'artificialisation
 * \details La fonction va personnaliser l'ouverture du projet QGiS en écrivant un script Python dans un dossier
 *          temporaire (\a C:\temp\qgis_projet_custom.py ). Le lancement de ce script est inscrit dans le fichier batch
 *          lors de l'exécution de QGiS (\a \release\QGiS_3.4.5\usbgis\apps\qgis\bin\qgis-ltr.bat ).\n
 *          Ces spécifications vont principalement concerner les paramètres suivants : zoom sur le territoire concerné,
 *          ouverture d'un fond raster en WMS, ouverture de la couche vectorielle des parcelles, et affichage des parcelles
 *          ayant au moins 1 local par hectare.
 * \param   territoire Classe \link Territoire \endlink qui donne accès à sa fonction \link Territoire::Emprise_communale \endlink
 * \return  void
 */
void Customisation_projet_QGiS(const Territoire * territoire)
{
    /* Vérification de l'existence d'un répertoire temporaire, sinon création */
    QDir repertoire("C:\\temp");
    if (!repertoire.exists())
        repertoire.mkdir("C:\\temp");

    /* Ouverture en écriture du fichier de script */
    QString chemin = "C:/temp/qgis_projet_custom.py";

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly))
        qDebug() << chemin;

    QTextStream texte(&fichier);

    /* Complétude du script
       -------------------- */

    /* Bibliothèques */
    texte << "from qgis.utils import iface" << endl;
    texte << "from qgis.core import *" << endl;
    texte << "from PyQt5.QtGui import QColor" << endl;

    /* Message d'accueil */
    texte << "iface.messageBar().pushMessage(\"DDT 57\", \"Bienvenue sur le projet d'observatoire du foncier...\")" << endl;
    /* Ouverture du projet */
    texte << "qgs = QgsApplication([], False)" << endl;
    texte << "qgs.initQgis()" << endl;
    texte << "project = QgsProject.instance()" << endl;
    texte << "project.read(\"" + QCoreApplication::applicationDirPath() + "/databank/cartographie/projet_qgis.qgz\")" << endl;
    /* Vue carto localisée sur le secteur géographique de la station hydro */
    texte << "qgis.utils.iface.mapCanvas().setExtent(QgsRectangle(" << territoire->Emprise_communale().second[0] << ","
                                                                    << territoire->Emprise_communale().second[1] << ","
                                                                    << territoire->Emprise_communale().second[2] << ","
                                                                    << territoire->Emprise_communale().second[3] << "))" << endl;
    /* Projection = Lambert 93 Bornes Europe = EPSG 2154 */
    texte << "lambert93 = QgsCoordinateReferenceSystem(2154, QgsCoordinateReferenceSystem.PostgisCrsId)\n"
             "qgis.utils.iface.mapCanvas().setDestinationCrs(lambert93)" << endl;
    /* Référentiels WMS */
    texte << QString("uri = 'crs=EPSG:2154&featureCount=10&format=image/png&layers=default&styles=&url=https://osm.geograndest.fr/mapcache/'") << endl;
    texte << QString("qgis.utils.iface.addRasterLayer(uri, 'Open Street Map', 'wms')") << endl;
    /* Couches SIG de type vecteur (parcelles du pnb10) */
    texte << QString("couche=iface.addVectorLayer(\"" + QCoreApplication::applicationDirPath() + "/databank/cartographie/pnb10_parcelle/d"+ territoire->Departement() + "_fftp_2019_pnb10_parcelle_nlocal.shp\", \"pnb10_parcelle\",\"ogr\")") << endl;
    /* Sélection des parcelles contenant moins d'1 local par ha, pour un affichage transparent */
    texte << QString("couche.selectByExpression(\"\\\"loc_par_ha\\\"<1\")") << endl;
    texte << QString("iface.mapCanvas().setSelectionColor(QColor(\"transparent\"))") << endl;
    /* Finalisation */
    texte << "qgis.utils.iface.mapCanvas().refresh()";

    fichier.close();
}

/**
 * \brief      SLOT - Accès au module cartographie de l'artificialisation des sols
 * \details    La fonction est connectée à un bouton (\a ui->pushButton_cartographie ); elle traite l'accès au logiciel
 *             SIG (<em> QGiS version 3.4.5 </em>) et à son ouverture personnalisée.
 * \param      bool
 * \return     void
 */
void FenetrePrincipale::Cartographie(bool)
{
    /* Customisation du projet QGiS */
    territoire->Evaluation_emprise_communale();
    Customisation_projet_QGiS(territoire);

    /* Exécution de QGiS */
    QProcess *process = new QProcess(this);
    QString chemin = QDir::toNativeSeparators(QCoreApplication::applicationDirPath())+ "\\QGiS_3.4.5\\usbgis\\apps\\qgis\\bin\\";
    process->setWorkingDirectory(chemin);
    process->start(chemin + "qgis-ltr.bat", QStringList());
}

/**
 * \brief      SLOT - Accès au menu d'impression d'un pdf de synthèse
 * \details    La fonction est connectée à une \a QAction (\a ui->action_imprimer_pdf ) du \a QMenu \a ui->menu_export,
 *             et produit une fichier pdf comportant les tableaux et graphiques de la requête utilisateur.
 * \param      void
 * \return     void
 */
void FenetrePrincipale::Menu_imprimer_pdf(void)
{
    /* Insertion des graphiques sous les tableaux de résultats */
    QList<QwtPlot *> graphiques;
    graphiques << graph_bati_cumul << graph_bati_courant << graph_conso_foncier;

    foreach (QwtPlot * plot, graphiques) {
        QwtPlotRenderer *plotRenderer = new QwtPlotRenderer();
        QImage *image = new QImage(665, 500, QImage::Format_ARGB32_Premultiplied);
        plotRenderer->renderTo(plot, (*image));

        QTextCursor cursor = ui->textBrowser_resultat->textCursor();
        cursor.insertText(QString("\n\n"));
        cursor.insertImage(*image);
    }

    /* Définition du nom du fichier pdf et de son répertoire (par défaut le bureau) */
    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer fichier pdf", QDir::homePath()+"/Desktop/" + territoire->Granularite() + "_" + territoire->Geographie().second, " PDF (*.pdf)");

    /* Impression */
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4Extra);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fichier);

    QTextDocument *document = new QTextDocument();
    document = ui->textBrowser_resultat->document();
    document->print(&printer);
}

/**
 * \brief      SLOT - Accès au menu d'impression au format html
 * \details    La fonction est connectée à une \a QAction (\a ui->action_imprimer_doc ) du \a QMenu \a ui->menu_export,
 *             et produit une fichier html comportant les tableaux de la requête utilisateur.
 * \param      void
 * \return     void
 */

void FenetrePrincipale::Menu_imprimer_doc(void)
{
    /* Définition du nom du fichier pdf et de son répertoire (par défaut le bureau) */
    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer fichier html", QDir::homePath()+"/Desktop/" + territoire->Granularite() + "_" + territoire->Geographie().second, " HTML (*.html)");

    /* Impression */
    QTextDocument *document = new QTextDocument();
    document = ui->textBrowser_resultat->document();

    QTextDocumentWriter fileWriter(fichier);
    fileWriter.setFormat("HTML");
    fileWriter.write(document);
}

/**
 * \brief      SLOT - Accès au menu d'export en jpeg des 3 graphiques
 * \details    La fonction est connectée à une \a QAction (\a ui->action_exporter_jpeg ) du \a QMenu \a ui->menu_export,
 *             et produit 3 fichiers jpeg comportant chaque graphique de la requête utilisateur.
 * \param      void
 * \return     void
 */
void FenetrePrincipale::Menu_exporter_jpeg(void)
{    
    QList<QwtPlot *> graphiques;
    graphiques << graph_bati_cumul << graph_bati_courant << graph_conso_foncier;
    QList<QString> noms;
    noms << "bati_cumul" << "bati_courant" << "conso";

    /* Définition du nom du fichier pdf et de son répertoire (par défaut le bureau) */
    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer fichier jpeg", QDir::homePath()+"/Desktop/" + territoire->Granularite() + "_" + territoire->Geographie().second + "_xxx", " JPG (*.jpg)");

    /* Export */
    QwtPlotRenderer *plotRenderer = new QwtPlotRenderer();
    for (int i(0); i < graphiques.size(); ++i){
        QString fichier_modif(fichier);
        fichier_modif.replace("xxx", noms.at(i));
        plotRenderer->renderDocument(graphiques.at(i), fichier_modif, QSizeF(300, 200), 120);
    }
    delete plotRenderer;
}

/**
 * \brief      SLOT - Accès au menu d'aide
 * \details    La fonction est connectée à 2 \a QAction (\a ui->action_manuel_utilisation et \a ui->action_methodologie )
 *             du \a QMenu \a ui->menu_aide; elle ouvre soit le pdf du manuel d'utilisation de l'IHM, soit celui de la
 *             méthode de calcul des indicateurs de la consommation foncière.
 * \param      void
 * \return     void
 */
void FenetrePrincipale::Menu_aide(void)
{
    QAction * action = qobject_cast<QAction *>(sender());

    QUrl url;
    if (action->text().contains("manuel", Qt::CaseInsensitive))
        url.setUrl("file:///databank/documentation/manuel_utilisation.pdf");
    else if (action->text().contains("méthode", Qt::CaseInsensitive))
        url.setUrl("file:///databank/documentation/methode.pdf");
    else
        QDesktopServices::openUrl(QUrl(action->whatsThis()));

    if (!url.isEmpty()){
        QUrl url_fichier = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + url.toLocalFile());
        QDesktopServices::openUrl(url_fichier);
    }
}

