#include "salarios.h"
#include "ui_salarios.h"

Salarios::Salarios(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Salarios)
{
    ui->setupUi(this);
    m_controlador = new Controlador();
}

Salarios::~Salarios()
{
    delete ui;
}


void Salarios::on_cmdCalcular_clicked()
{
    calcular();
}

void Salarios::limpiar()
{
    ui->inNombre->setText("");
    ui->inHoras->setValue(0);
    ui->inMatutino->setChecked(true);
    ui->inNombre->setFocus();
}

void Salarios::guardar()
{
    // Abrir cuadro de diálogo para seleccionar ubicación y nombre del archivo.
    QString nombreArchivo = QFileDialog::getSaveFileName(this,
                                                         "Guardar archivo",
                                                         QDir::home().absolutePath(),
                                                         "Archivos de salarios (*.slr)");

    // Crear un objeto QFile
    QFile archivo(nombreArchivo);
    // Abrirlo para escritura
    if(archivo.open(QFile::WriteOnly | QFile::Truncate)){
        // Crear un 'stream' de texto
        QTextStream salida(&archivo);
        // Enviar los datos del resultado a la salida
        salida << ui->outResultado->toPlainText();
        // Mostrar 5 segundo que todo fue bien
        ui->statusbar->showMessage("Datos almacenados en " + nombreArchivo, 5000);
    }else {
        // Mensaje de error si no se puede abrir el archivo
        QMessageBox::warning(this,
                             "Guardar datos",
                             "No se pudo guardar el archivo");
    }
    // Cerrar el archivo
    archivo.close();

}

void Salarios::abrir()
{
    // Abrir cuadro de diálogo para seleccionar ubicación y nombre del archivo.
    QString nombreArchivo = QFileDialog::getOpenFileName(this,
                                                         "Abrir archivo",
                                                         QDir::home().absolutePath(),
                                                         "Archivos de salarios (*.slr)");

    // Crear un objeto QFile
    QFile archivo(nombreArchivo);
    // Abrirlo para lectura
    if(archivo.open(QFile::ReadOnly)){
        // Crear un 'stream' de texto
        QTextStream entrada(&archivo);
        // Leer todo el contenido del archivo
        QString datos = entrada.readAll();
        // Cargar el contenido al área de texto
        ui->outResultado->clear();
        ui->outResultado->setPlainText(datos);
        // Mostrar 5 segundo que todo fue bien
        ui->statusbar->showMessage("Datos leidos desde " + nombreArchivo, 5000);
    }else {
        // Mensaje de error si no se puede abrir el archivo
        QMessageBox::warning(this,
                             "Abrir datos",
                             "No se pudo abrir el archivo");
    }
    // Cerrar el archivo
    archivo.close();
}


void Salarios::on_actionCalcular_triggered()
{
    calcular();
}

void Salarios::calcular()
{
    // Obteber datos de la GUI
    QString nombre = ui->inNombre->text();
    int horas = ui->inHoras->value();
    TipoJornada jornada;
    if (ui->inMatutino->isChecked()){
        jornada = TipoJornada::Matutina;
    } else if (ui->inVespertina->isChecked()){
        jornada = TipoJornada::Vespertina;
    } else {
        jornada = TipoJornada::Nocturna;
    }

    // Validar datos correctos
    if (nombre == "" || horas == 0){
        /*
        QMessageBox msgBox;
        msgBox.setText("El nombre o el número de horas está vacío");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        */
        QMessageBox::warning(this,"Advertencia","El nombre o el número de horas está vacío");
        return;
    }

    // Agregar obrero al controlador
    m_controlador->agregarObrero(nombre, horas, jornada);
    // Calcular
    if (m_controlador->calcularSalario()){
        // muestra los resultados de los calculos del obrero
        ui->outResultado->appendPlainText(m_controlador->obrero()->toString());
        // limpiar la interfaz
        limpiar();
        // Mostrar mensaje por 5 segundos en la barra de estado
        ui->statusbar->showMessage("calculos procesados para " + nombre, 5000);
    }else {
        QMessageBox::critical(
                    this,
                    "Error",
                    "Error al calcular el salario.");
    }
}


void Salarios::on_actionGuardar_triggered()
{
    guardar();
}


void Salarios::on_actionNuevo_triggered()
{
    limpiar();
    ui->outResultado->clear();
}


void Salarios::on_actionAbrir_triggered()
{
    abrir();
}


void Salarios::on_actionAcerca_de_Salarios_triggered()
{
    // Crear un objeto de la ventana que queremos invocar
    Acerca *dialogo = new Acerca(this);
    // Enviar parámetro a la ventana
    dialogo->setVersion(VERSION);
    // Mostrar la ventana (diálogo) MODAL
    dialogo->exec();
    // Obtener datos de la ventana
    qDebug() << dialogo->valor();
}

