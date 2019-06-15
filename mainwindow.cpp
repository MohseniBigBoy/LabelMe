#include "mainwindow.h"
#include "ui_mainwindow.h"


//#define IGNORE_LOGIN_WINDOW


QString LOG_FILE_PATH;

QStringList prc_imgs;   // processed images
QStringList unprc_imgs; // unprocessed images


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("LabelMe");
    this->setGeometry(
                QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,
                                     this->size(), qApp->desktop()->availableGeometry()  )   );

    QMenuBar *menu_bar=new QMenuBar(this);
    QMenu *menu_help = new QMenu("&Help",menu_bar);
    menu_help->addAction(ui->action_Help);
    menu_bar->addMenu(menu_help);
    this->layout()->setMenuBar(menu_bar);

    ui->grpBox_op_sel->setEnabled(false);
    ui->grpBox_path->setEnabled(false);
    ui->btn_enter->setEnabled(false);


#ifdef IGNORE_LOGIN_WINDOW
    //                opt_drew_roi=true;
    opt_review_edit=true;

    save_res_img=true;

    load_img_path="/home/bigboy/Pictures/animal_pics";
    store_img_path="/home/bigboy/Desktop/test/res_images";
    load_text_path="/home/bigboy/Desktop/test/text/res.txt";
    store_text_path="/home/bigboy/Desktop/test_cls.txt";
    IN_IMAGES_NAME=fetch_image_file_names(load_img_path);
    input_text_names=fetch_image_names_from_text_file(load_text_path);

    on_btn_enter_clicked();

#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::let_to_enter()
{
    ui->btn_enter->setEnabled(false);

    if(valid_load_img_path==false)
        return;

    if(save_res_img==true && valid_store_img_path==false)
        return;

    if( (opt_drew_roi==true && valid_store_text_path==true) || (opt_review_edit==true && valid_load_text_path==true) )
    {
        ui->btn_enter->setEnabled(true);
    }
    else
    {
        //        qDebug()<<"Undefined operation!";
        qDebug()<<"IN_IMAGES_NAME.size "<<IN_IMAGES_NAME.size();
    }

    //    if( valid_load_img_path && (valid_store_text_path || (valid_load_text_path && valid_store_img_path) ) )
    //    {
    //        ui->btn_enter->setEnabled(true);
    //        return;
    //    }
}


void MainWindow::clear_input_data(){

    ui->btn_enter->setEnabled(false);

    load_img_path="";
    valid_load_img_path=false;
    store_img_path="";
    valid_store_img_path=false;
    load_text_path="";
    valid_load_text_path=false;
    store_text_path="";
    valid_store_text_path=false;

    opt_drew_roi=false;
    opt_review_edit=false;
    //    save_res_img=false;

    ui->lnE_load_img->clear();
    ui->lnE_load_text->clear();
    ui->lnE_store_img->clear();
    ui->lnE_store_text->clear();
}


void MainWindow::on_lnEusername_textEdited(const QString &arg1)
{
    if(arg1.size()>0)
    {
        ui->grpBox_op_sel->setEnabled(true);
    }
    else
    {
        ui->rdbtn_roi->setAutoExclusive(false);
        ui->rdbtn_edit->setAutoExclusive(false);
        ui->rdbtn_roi->setChecked(false);
        ui->rdbtn_edit->setChecked(false);
        ui->rdbtn_roi->setAutoExclusive(true);
        ui->rdbtn_edit->setAutoExclusive(true);

        save_res_img=false;
        ui->chBox_save_result_images->setChecked(false);

        ui->grpBox_op_sel->setEnabled(false);

        ui->lnE_load_img->clear();
        ui->lnE_load_text->clear();
        ui->lnE_store_img->clear();
        ui->lnE_store_text->clear();

        ui->grpBox_path->setEnabled(false); // disable btn_load_img  btn_load_text  btn_store_img  btn_store_text
        ui->btn_enter->setEnabled(false);

        clear_input_data();
    }
}


//void MainWindow::on_rdbtn_cls_clicked(bool checked)
//{
//    clear_input_data();
//    if(checked==true)
//    {
//        ui->grpBox_path->setEnabled(true);

//        ui->btn_load_img->setEnabled(true);
//        ui->lnE_load_img->setEnabled(true);
//        ui->btn_store_text->setEnabled(true);
//        ui->lnE_store_text->setEnabled(true);
//        ui->chBox_save_result_images->setEnabled(true);
//        ui->btn_store_img->setEnabled(save_res_img);
//        ui->lnE_store_img->setEnabled(save_res_img);
//        ui->btn_load_text->setEnabled(false);
//        ui->lnE_load_text->setEnabled(false);
//    }
//}


void MainWindow::on_rdbtn_roi_clicked(bool checked)
{
    clear_input_data();
    if(checked==true)
    {
        opt_drew_roi=true;
        ui->grpBox_path->setEnabled(true);
        ui->btn_load_img->setEnabled(true);
        ui->lnE_load_img->setEnabled(true);
        ui->btn_store_text->setEnabled(true);
        ui->lnE_store_text->setEnabled(true);
        ui->chBox_save_result_images->setEnabled(true);
        ui->btn_store_img->setEnabled(save_res_img);
        ui->lnE_store_img->setEnabled(save_res_img);
        ui->btn_load_text->setEnabled(false);
        ui->lnE_load_text->setEnabled(false);
    }
}

void MainWindow::on_rdbtn_edit_clicked(bool checked)
{
    clear_input_data();
    if(checked==true)
    {
        opt_review_edit=true;
        ui->grpBox_path->setEnabled(true);
        ui->btn_load_img->setEnabled(true);
        ui->lnE_load_img->setEnabled(true);
        ui->chBox_save_result_images->setEnabled(true);
        ui->btn_store_img->setEnabled(save_res_img);
        ui->lnE_store_img->setEnabled(save_res_img);
        ui->btn_load_text->setEnabled(true);
        ui->lnE_load_text->setEnabled(true);
        ui->btn_store_text->setEnabled(false);
        ui->lnE_store_text->setEnabled(false);
    }
}

void MainWindow::on_chBox_save_result_images_clicked(bool checked)
{ 
    if(checked==true)
        save_res_img=true;
    else
        save_res_img=false;

    ui->btn_store_img->setEnabled(save_res_img);
    ui->lnE_store_img->setEnabled(save_res_img);

    ui->lnE_store_img->clear();
    let_to_enter();
}


void MainWindow::on_btn_load_img_clicked()
{

    load_img_path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Select Input Images Path"), QDir::homePath()/*currentPath()*/));

    QFileInfo Folder(load_img_path);
    if(!Folder.exists() || load_img_path.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Error", "Please choose a valid path !",QMessageBox::Ok);
        valid_load_img_path=false;
        ui->lnE_load_img->clear();
        return;
    }

    valid_load_img_path=true;
    ui->lnE_load_img->setText(load_img_path+"/");
    IN_IMAGES_NAME=fetch_image_file_names(load_img_path);
    if(IN_IMAGES_NAME.size()<=0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "Error", "The given path is empty !",QMessageBox::Ok);
        valid_load_img_path=false;
        ui->lnE_load_img->clear();
    }

    let_to_enter();
}

void MainWindow::on_btn_load_text_clicked()
{
    load_text_path = QFileDialog::getOpenFileName(this, tr("Find Input Text File"),QDir::homePath(), tr("Text Files (*.txt)"));

    QMessageBox::StandardButton reply;
    QFileInfo File(load_text_path);
    if(!File.exists())
    {
        reply = QMessageBox::critical(this, "Error", "Please choose a valid path !",QMessageBox::Ok);
        valid_load_text_path=false;
        ui->lnE_load_text->clear();
        return;
    }
    valid_load_text_path=true;
    ui->lnE_load_text->setText(load_text_path);

    input_text_names=fetch_image_names_from_text_file(load_text_path);
    if(input_text_names.size()<=0)
    {
        reply = QMessageBox::critical(this, "Error", "The given path is empty !",QMessageBox::Ok);
        valid_load_text_path=false;
        ui->lnE_load_text->clear();
    }

    let_to_enter();
}

void MainWindow::on_btn_store_img_clicked()
{

    store_img_path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Select Output Images Path"), QDir::homePath()/*currentPath()*/));

    QFileInfo Folder(store_img_path+"/");
    if(!Folder.exists() || store_img_path.isEmpty() )
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "Error", "Please choose a valid path !",QMessageBox::Ok);
        valid_store_img_path=false;
        ui->lnE_store_img->clear();
        return;
    }

    ui->lnE_store_img->setText(store_img_path+"/");

    valid_store_img_path=true;

    let_to_enter();
}

void MainWindow::on_btn_store_text_clicked()
{
    //    store_text_path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Select Ouput Text File Path"), QDir::homePath()/*currentPath()*/));


    //    store_text_path = QFileDialog::getOpenFileName(this, tr("Result Text File"),"", tr("Text Files (*.txt)"));
    store_text_path = QFileDialog::getSaveFileName(this,
                                                   tr("Select Result Text File"),QDir::homePath(),
                                                   tr("Text Files (*.txt)"), 0,
                                                   QFileDialog::DontConfirmOverwrite);

    if(store_text_path.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "Error", "Please choose a valid path!",QMessageBox::Ok);
        valid_store_text_path=false;
        ui->lnE_store_text->clear();
        return;
    }
    QString prefer_suffix=".txt";
    QStringRef suffix(&store_text_path, store_text_path.length() - prefer_suffix.length(), prefer_suffix.length());
    if( suffix != prefer_suffix)
        store_text_path +=prefer_suffix;

    ui->lnE_store_text->setText(store_text_path);
    valid_store_text_path=true;

    let_to_enter();
}

void MainWindow::on_btn_enter_clicked()
{
    this->close();


    //    qDebug()<< "  opt_drew_roi:"<<opt_drew_roi <<
    //              "  opt_review_edit:"<<opt_review_edit <<
    //              "  save_res_img:"<<save_res_img ;


    QFile file(ui->lnE_store_text->text());
    if(!file.exists())
    {
        file.open(QIODevice::WriteOnly);
        file.close();
    }


    unprc_imgs=IN_IMAGES_NAME;
    if(opt_drew_roi == true )
        prc_imgs=fetch_image_names_from_text_file(store_text_path);
    else
        prc_imgs.clear();

    foreach (QString img_name, prc_imgs)
        if(unprc_imgs.contains(img_name) )
            unprc_imgs.removeOne(img_name);



    if(unprc_imgs.size()<1)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::information(this, "Information", "The images have recently been processed! ",QMessageBox::Ok);
        exit(EXIT_SUCCESS);
    }


    QString LogFile_dir = QDir::currentPath()+"/Log_files/";
    if(! QDir(LogFile_dir).exists())
    {
        QDir(LogFile_dir).mkpath(".");
    }
    else // check if
    {
        QStringList log_file_content=fetch_file_names(LogFile_dir,{"*.txt"});
        int extra_files_count=log_file_content.size() - MAX_LOG_FILES_COUNT;
        while(extra_files_count>=0)
        {
            QFile::remove(LogFile_dir+log_file_content[extra_files_count]);
            extra_files_count--;
        }
    }

    QString app_execute_time=QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss");

    LOG_FILE_PATH=LogFile_dir+app_execute_time+".txt";

    QFile log_file(LOG_FILE_PATH);
    log_file.open(QIODevice::WriteOnly);

    QTextStream log_data(&log_file);
    log_data <<"\nThis log file has been created at "<<app_execute_time;
    log_data <<"\nUsername:           "<<ui->lnEusername->text();
    log_data <<"\nOperation:          "<< (ui->rdbtn_edit->isChecked() ? ("Review & Edit") : ("Crop ROI"));
    log_data <<"\nLoad images path:   "<< (ui->lnE_load_img->isEnabled() ? ui->lnE_load_img->text() : "-");
    log_data <<"\nStore images path:  "<< (ui->lnE_store_img->isEnabled() ? ui->lnE_store_img->text() : "-");
    log_data <<"\nLoad text path:     "<< (ui->lnE_load_text->isEnabled() ? ui->lnE_load_text->text() : "-");
    log_data <<"\nStore text path:    "<< (ui->lnE_store_text->isEnabled() ? ui->lnE_store_text->text() : "-");
    log_data <<"\n\n************************************************\n";
    log_data << endl;

    log_file.close();


    ROI_SELECTION *crop_win = new ROI_SELECTION();
    crop_win->show();
}


void MainWindow::on_action_Help_triggered()
{
    help_win.setCurrentIndex(0);
    help_win.show();
}
