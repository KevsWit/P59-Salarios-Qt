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
        salida << "\nTOTALES:\n";
        salida << "\nSalario Bruto: $" + QString::number(m_controlador->totalSB(), 'f', 2);
        salida << "\nIESS: $" + QString::number(m_controlador->totalIESS(), 'f', 2);
        salida << "\nSalario Neto: $" + QString::number(m_controlador->totalSN(), 'f', 2);

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
        // Limpiar
        limpiar();
        ui->outResultado->clear();
        // resetear los valores de los totales
        m_controlador->setTotalSB(0);
        m_controlador->setTotalIESS(0);
        m_controlador->setTotalSN(0);
        // limpiar las lineas de salida de texto
        ui->outSalarioBruto->clear();
        ui->outIESS->clear();
        ui->outSalarioNeto->clear();
        // Leer los datos
        int bandera = 0;
        do{
            // Cargar el contenido al área de texto
            QString linea = entrada.readLine();
            if (linea != "TOTALES:"){
                ui->outResultado->appendPlainText(linea);
            }else{
                bandera = 1;
            }
        }while(bandera != 1);
        // Cargar el contenido las lineas de textos
        QString linea = entrada.readLine();
        linea = entrada.readLine();
        int pos = linea.indexOf('$')+1;
        QString strSB = linea.mid(pos);
        ui->outSalarioBruto->setText(" $ " + strSB);
        linea = entrada.readLine();
        pos = linea.indexOf('$')+1;
        QString strIESS = linea.mid(pos);
        ui->outIESS->setText(" $ " + strIESS);
        linea = entrada.readLine();
        pos = linea.indexOf('$')+1;
        QString strSN = linea.mid(pos);
        ui->outSalarioNeto->setText(" $ " + strSN);
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
        // se aumenta los valores a los totales respectivos
        m_controlador->setTotalSB(m_controlador->totalSB() + m_controlador->obrero()->salarioBruto());
        ui->outSalarioBruto->setText(" $ " + QString::number(m_controlador->totalSB(), 'f', 2));
        m_controlador->setTotalIESS(m_controlador->totalIESS() + m_controlador->obrero()->descuento());
        ui->outIESS->setText(" $ " + QString::number(m_controlador->totalIESS(), 'f', 2));
        m_controlador->setTotalSN(m_controlador->totalSN() + m_controlador->obrero()->salarioNeto());
        ui->outSalarioNeto->setText(" $ " + QString::number(m_controlador->totalSN(), 'f', 2));
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
    // resetear los valores de los totales
    m_controlador->setTotalSB(0);
    m_controlador->setTotalIESS(0);
    m_controlador->setTotalSN(0);
    // limpiar las lineas de salida de texto
    ui->outSalarioBruto->clear();
    ui->outIESS->clear();
    ui->outSalarioNeto->clear();
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

