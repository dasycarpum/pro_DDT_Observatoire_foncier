#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Initialisation du territoire d'étude */
    ui->radioButton_commune->setChecked(true);
    QStringList liste_geographies = Territoire::Liste_libelles_geographies("Commune");
    ui->comboBox_geographie->addItems(liste_geographies);

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
}

void FenetrePrincipale::Validation_des_saisies(bool)
{
    /* Territoire d'étude */
    QPair<QString, QString> geographie;
    geographie.second = ui->comboBox_geographie->currentText();
    territoire = new Territoire("Commune", geographie);

    /* Période d'observation */
    periode = new Periode();
    periode->Annee_debut(ui->spinBox_an_debut->value());
    periode->Annee_fin(ui->spinBox_an_fin->value());
    periode->Pas_de_temps(ui->spinBox_an_pas->value());

    /* Calcul des occupations du sol par année */
    Algorithme *algorithme = new Algorithme(territoire, periode);

    /* Affichage des tableaux de résultats */
    Affichage_tableau_occupation(algorithme);

}

void FenetrePrincipale::Affichage_tableau_occupation(const Algorithme * algo)
{
    /* Rafraichissement de la zone d'édition */
    ui->textBrowser_resultat->clear();

    /* Territoire d'étude et millésime des données */
    ui->textBrowser_resultat->append(QString("<TABLE BORDER WIDTH=600 CELLSPACING=3 ALIGN=center>"
                                                "<TR>"
                                                    "<TD align=center><h2>%1 : %2</h2><h3>Données foncières au 1<SUP>er</SUP> janvier %3</h2></TD>"
                                                "</TR>"
                                             "</TABLE><br>")
                                                .arg(territoire->Granularite())
                                                .arg(territoire->Geographie().second)
                                                .arg(periode->Annee_fin() + 1));

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
