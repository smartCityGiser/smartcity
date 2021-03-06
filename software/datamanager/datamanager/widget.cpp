#include "widget.h"
#include "ui_widget.h"
#include <QtSql/QtSql>
#include <QtGui>
#include "pieview.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon("D:\\work\\smartcity\\smartcity\\software\\datamanager\\datamanager\\Resources\\windowIcon.jpg"));

    ui->stackedWidget->setCurrentIndex(0);

    QSqlQueryModel *typeModel = new QSqlQueryModel(this);
    typeModel->setQuery("select name from type");
    ui->geomTypeComboBox->setModel(typeModel);

    on_sellCancelBtn_clicked();
    on_dataCancelBtn_clicked();
    on_newCancelBtn_clicked();

    showDailyList();

    ui->typeComboBox->setModel(typeModel);
    ui->dataTypeComboBox->setModel(typeModel);
    ui->newTypeComboBox->setModel(typeModel);

    createChartModelView();

}

Widget::~Widget()
{
    delete ui;
}

// 当数据的几何类型改变时
void Widget::on_geomTypeComboBox_currentIndexChanged(QString type)
{
    if (type == "请选择类型") {
        // 进行其他部件的状态设置
        on_sellCancelBtn_clicked();
    } else {
        ui->sellBrandComboBox->setEnabled(true);
        QSqlQueryModel *sellBrandModel = new QSqlQueryModel(this);
        sellBrandModel->setQuery(QString("select name from metadata where type=(select id from type where name='%1' )").arg(type));
        ui->sellBrandComboBox->setModel(sellBrandModel);
        ui->sellCancelBtn->setEnabled(true);
    }
}

// 已有数据入库的数据类型改变时
void Widget::on_dataTypeComboBox_currentIndexChanged(QString type)
{
    if (type == "请选择类型") {
        // 进行其他部件的状态设置
        on_dataCancelBtn_clicked();
    } else {
        ui->dataBrandComboBox->setEnabled(true);
        QSqlQueryModel *goodBrandModel = new QSqlQueryModel(this);
        goodBrandModel->setQuery(QString("select name from metadata where type=(select id from type where name='%1')").arg(type));
        ui->dataBrandComboBox->setModel(goodBrandModel);
        ui->dataCancelBtn->setEnabled(true);
    }
}

// 新数据入库类型改变时
void Widget::on_newTypeComboBox_currentIndexChanged(QString type)
{
    if (type == "请选择类型") {
        // 进行其他部件的状态设置
        on_newCancelBtn_clicked();
    } else {
        ui->newBrandLineEdit->setEnabled(true);
        ui->newSumLineEdit_2->setEnabled(true);
        ui->newSumLineEdit->setEnabled(true);

        ui->newBrandLineEdit->setFocus();
    }
}

// 出售数据的类型改变时
void Widget::on_sellBrandComboBox_currentIndexChanged(QString brand)
{
    ui->sellNumSpinBox->setValue(0);
    ui->sellNumSpinBox->setEnabled(false);
    ui->sellSumLineEdit->clear();
    ui->sellSumLineEdit->setEnabled(false);
    ui->sellOkBtn->setEnabled(false);

    QSqlQuery query;
    query.exec(QString("select geom_wkt from metadata where name='%1' and type=(select id from type where name='%2')")
               .arg(brand).arg(ui->geomTypeComboBox->currentText()));
    query.next();
    ui->sellPriceLineEdit->setEnabled(true);
    ui->sellPriceLineEdit->setReadOnly(true);
    ui->sellPriceLineEdit->setText(query.value(0).toString());

   
    query.exec(QString("select des from metadata where name='%1' and type=(select id from type where name='%2')")
        .arg(brand).arg(ui->geomTypeComboBox->currentText()));
    query.next();
    ui->sellSumLineEdit->setEnabled(true);
    ui->sellSumLineEdit->setReadOnly(true);
    QString mm = query.value(0).toString();
    ui->sellSumLineEdit->setText(query.value(0).toString());
    

    query.exec(QString("select num from metadata where name='%1' and type=(select id from type where name='%2')")
               .arg(brand).arg(ui->geomTypeComboBox->currentText()));
    query.next();
    int num = query.value(0).toInt();

    if (num == 0) {
        QMessageBox::information(this, tr("提示"), tr("该数据已经售完！"), QMessageBox::Ok);
    } else {
        ui->sellNumSpinBox->setEnabled(true);
        ui->sellNumSpinBox->setMaximum(num);
        ui->sellLastNumLabel->setText(tr("剩余数量：%1").arg(num));
        ui->sellLastNumLabel->setVisible(true);
    }
}

// 已有数据入库的类型改变时
void Widget::on_dataBrandComboBox_currentIndexChanged(QString brand)
{
    ui->dataNumSpinBox->setValue(0);
    ui->dataNumSpinBox->setEnabled(true);
    
    ui->dataOkBtn->setEnabled(false);

    QSqlQuery query;
    query.exec(QString("select geom_wkt from metadata where name='%1' and type=(select id from type where name='%2')")
               .arg(brand).arg(ui->dataTypeComboBox->currentText()));
    query.next();
    ui->dataPriceLineEdit->setEnabled(true);
    ui->dataPriceLineEdit->setReadOnly(true);
    ui->dataPriceLineEdit->setText(query.value(0).toString());
    
    query.exec(QString("select des from metadata where name='%1' and type=(select id from type where name='%2')")
        .arg(brand).arg(ui->dataTypeComboBox->currentText()));
    query.next();
    ui->dataSumLineEdit->clear();
    ui->dataSumLineEdit->setEnabled(true);
     ui->dataSumLineEdit->setText(query.value(0).toString());
}

// 新数据类型改变时
void Widget::on_newBrandLineEdit_textChanged(QString str)
{
    if (str == "") {
        ui->newCancelBtn->setEnabled(false);
        ui->newNumSpinBox->setEnabled(false);
        ui->newSumLineEdit->setEnabled(false);
        ui->newSumLineEdit->clear();
        ui->newOkBtn->setEnabled(false);
    } else {
        ui->newCancelBtn->setEnabled(true);
        ui->newNumSpinBox->setEnabled(true);
        ui->newSumLineEdit->setEnabled(true);
        ui->newSumLineEdit_2->setEnabled(true);
        ui->newOkBtn->setEnabled(true);
    }
}

// 出售数据数量改变时
void Widget::on_sellNumSpinBox_valueChanged(int value)
{
    if (value == 0) {
        ui->sellOkBtn->setEnabled(false);
    } else {
        qreal sum = value * ui->sellPriceLineEdit->text().toInt();
        ui->sellOkBtn->setEnabled(true);
    }
}

// 已有数据入库数量改变时
void Widget::on_dataNumSpinBox_valueChanged(int value)
{
    if (value == 0) {
        ui->dataSumLineEdit->clear();
        ui->dataSumLineEdit->setEnabled(true);
        ui->dataOkBtn->setEnabled(false);
    } else {
        ui->dataSumLineEdit->setEnabled(true);
        qreal sum = value * ui->dataPriceLineEdit->text().toInt();
        ui->dataOkBtn->setEnabled(true);
    }

}

// 新数据单价改变时
void Widget::on_newPriceSpinBox_valueChanged(int value)
{
    qreal sum = value * ui->newNumSpinBox->value();
    ui->newSumLineEdit->setEnabled(true);

    ui->newOkBtn->setEnabled(true);
}

// 新数据数量
void Widget::on_newNumSpinBox_valueChanged(int value)
{
    ui->newOkBtn->setEnabled(true);
    ui->newSumLineEdit->setEnabled(true);
    ui->newSumLineEdit_2->setEnabled(true);
}

// 出售数据的取消按钮
void Widget::on_sellCancelBtn_clicked()
{
    ui->geomTypeComboBox->setCurrentIndex(0);
    ui->sellBrandComboBox->clear();
    ui->sellBrandComboBox->setEnabled(false);
    ui->sellPriceLineEdit->clear();
    ui->sellPriceLineEdit->setEnabled(false);
    ui->sellNumSpinBox->setValue(0);
    ui->sellNumSpinBox->setEnabled(false);
    ui->sellSumLineEdit->clear();
    ui->sellSumLineEdit->setEnabled(false);
    ui->sellOkBtn->setEnabled(false);
    ui->sellCancelBtn->setEnabled(false);
    ui->sellLastNumLabel->setVisible(false);
}

// 已有数据入库的取消按钮
void Widget::on_dataCancelBtn_clicked()
{
    ui->dataTypeComboBox->setCurrentIndex(0);
    ui->dataBrandComboBox->clear();
    ui->dataBrandComboBox->setEnabled(false);
    ui->dataPriceLineEdit->clear();
    ui->dataPriceLineEdit->setEnabled(false);
    ui->dataNumSpinBox->setValue(0);
    ui->dataNumSpinBox->setEnabled(false);
    ui->dataSumLineEdit->clear();
    ui->dataSumLineEdit->setEnabled(false);
    ui->dataOkBtn->setEnabled(false);
    ui->dataCancelBtn->setEnabled(false);
}

// 新数据入库的取消按钮
void Widget::on_newCancelBtn_clicked()
{
    ui->newTypeComboBox->setCurrentIndex(0);
    ui->newBrandLineEdit->clear();
    ui->newBrandLineEdit->setEnabled(false);
    ui->newNumSpinBox->setEnabled(false);
    ui->newNumSpinBox->setValue(1);
    ui->newSumLineEdit->setEnabled(false);
    ui->newOkBtn->setEnabled(false);
    ui->newCancelBtn->setEnabled(false);
}


// 出售数据的确定按钮
void Widget::on_sellOkBtn_clicked()
{
    QString type = ui->geomTypeComboBox->currentText();
    QString name = ui->sellBrandComboBox->currentText();
    int value = ui->sellNumSpinBox->value();
    // cellNumSpinBox的最大值就是以前的剩余量
    int last = ui->sellNumSpinBox->maximum() - value;

    QSqlQuery query;

    // 获取以前的销售量
    query.exec(QString("select allselled from metadata where name='%1' and type = (select id from type where name ='%2')")
               .arg(name).arg(type));
    query.next();
    int sell = query.value(0).toInt() + value;

    // 事务操作
    QSqlDatabase::database().transaction();
    bool rtn = query.exec(
                QString("update metadata set allselled=%1,num=%2 where name='%3'and type = (select id from type where name ='%4')")
                .arg(sell).arg(last).arg(name).arg(type));

    if (rtn) {
        QSqlDatabase::database().commit();
        QMessageBox::information(this, tr("提示"), tr("购买成功！"), QMessageBox::Ok);
        writeXml();
        showDailyList();
    } else {
        QSqlDatabase::database().rollback();
        QMessageBox::information(this, tr("提示"), tr("购买失败，无法访问数据库！"), QMessageBox::Ok);
    }
    on_sellCancelBtn_clicked();
}

// 已有数据入库的确定按钮
void Widget::on_dataOkBtn_clicked()
{
    QString type = ui->dataTypeComboBox->currentText();
    QString name = ui->dataBrandComboBox->currentText();
    int value = ui->dataNumSpinBox->value();

    QSqlQuery query;
    // 获取以前的总量
   query.exec(QString("select num from metadata where name='%1' and type=(select id from type where name='%2')")
       .arg(name).arg(type));
    query.next();
    int sum = query.value(0).toInt() + value;


    // 事务操作
    QSqlDatabase::database().transaction();
    bool rtn = query.exec(
                QString("update metadata set num=%1 where name='%2' and type= (select id from type where name='%3')")
                .arg(sum).arg(name).arg(type));
    if (rtn) {
        QSqlDatabase::database().commit();
        QMessageBox::information(this, tr("提示"), tr("入库成功！"), QMessageBox::Ok);
    } else {
        QSqlDatabase::database().rollback();
        QMessageBox::information(this, tr("提示"), tr("入库失败，无法访问数据库！"), QMessageBox::Ok);
    }
    on_dataCancelBtn_clicked();
}

// 新数据的确定按钮
void Widget::on_newOkBtn_clicked()
{
    QString type = ui->newTypeComboBox->currentText();
    QString brand = ui->newBrandLineEdit->text();
    qint16 num = ui->newNumSpinBox->value();
    QString bbox = ui->newSumLineEdit_2->text();
    QString des =ui->newSumLineEdit->text();

    QSqlQuery query;
    query.exec("select id from metadata");
    query.last();
    int id = query.value(0).toInt() + 1;


    query.exec(QString("select id from type where name = '%1'").arg(type));
    query.next();
    int type_id = query.value(0).toInt();


    // 事务操作
    QSqlDatabase::database().transaction();
    bool rtn = query.exec(QString("insert into metadata values(%1, '%2',%3, '%4', '%5',%6,%7)")
                .arg(id).arg(brand).arg(type_id).arg(des).arg(bbox).arg(num).arg(int(0)));
    if (rtn) {
        QSqlDatabase::database().commit();
        QMessageBox::information(this, tr("提示"), tr("入库成功！"), QMessageBox::Ok);
    } else {
        QSqlDatabase::database().rollback();
        QMessageBox::information(this, tr("提示"), tr("入库失败，无法访问数据库！"), QMessageBox::Ok);
    }

    on_newCancelBtn_clicked();
}

// 获取当前的日期或者时间
QString Widget::getDateTime(Widget::DateTimeType type)
{
    QDateTime datetime = QDateTime::currentDateTime();
    QString date = datetime.toString("yyyy-MM-dd");
    QString time = datetime.toString("hh:mm");
    QString dateAndTime = datetime.toString("yyyy-MM-dd dddd hh:mm");
    if(type == Date) return date;
    else if(type == Time) return time;
    else return dateAndTime;
}

// 读取XML文档
bool Widget::docRead()
{
    QFile file("data.xml");
    if(!file.open(QIODevice::ReadOnly))
        return false;
    if(!doc.setContent(&file)){
        file.close();
        return false;
    }
    file.close();
    return true;
}

// 写入xml文档
bool Widget::docWrite()
{
    QFile file("data.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    doc.save(out,4);
    file.close();
    return true;
}

// 将销售记录写入文档
void Widget::writeXml()
{
    // 先从文件读取
    if (docRead()) {
        QString currentDate = getDateTime(Date);
        QDomElement root = doc.documentElement();
        // 根据是否有日期节点进行处理
        if (!root.hasChildNodes()) {
            QDomElement date = doc.createElement(QString("日期"));
            QDomAttr curDate = doc.createAttribute("date");
            curDate.setValue(currentDate);
            date.setAttributeNode(curDate);
            root.appendChild(date);
            createNodes(date);
        } else {
            QDomElement date = root.lastChild().toElement();
            // 根据是否已经有今天的日期节点进行处理
            if (date.attribute("date") == currentDate) {
                createNodes(date);
            } else {
                QDomElement date = doc.createElement(QString("日期"));
                QDomAttr curDate = doc.createAttribute("date");
                curDate.setValue(currentDate);
                date.setAttributeNode(curDate);
                root.appendChild(date);
                createNodes(date);
            }
        }
        // 写入到文件
        docWrite();
    }
}

// 创建节点
void Widget::createNodes(QDomElement &date)
{
    QDomElement time = doc.createElement(QString("时间"));
    QDomAttr curTime = doc.createAttribute("time");
    curTime.setValue(getDateTime(Time));
    time.setAttributeNode(curTime);
    date.appendChild(time);

    QDomElement type = doc.createElement(QString("类型"));
    QDomElement brand = doc.createElement(QString("类型"));
    QDomElement price = doc.createElement(QString("单价"));
    QDomElement num = doc.createElement(QString("数量"));
    QDomElement sum = doc.createElement(QString("金额"));
    QDomText text;
    text = doc.createTextNode(QString("%1")
                              .arg(ui->geomTypeComboBox->currentText()));
    type.appendChild(text);
    text = doc.createTextNode(QString("%1")
                              .arg(ui->sellBrandComboBox->currentText()));
    brand.appendChild(text);
    text = doc.createTextNode(QString("%1")
                              .arg(ui->sellPriceLineEdit->text()));
    price.appendChild(text);
    text = doc.createTextNode(QString("%1")
                              .arg(ui->sellNumSpinBox->value()));
    num.appendChild(text);
    text = doc.createTextNode(QString("%1")
                              .arg(ui->sellSumLineEdit->text()));
    sum.appendChild(text);

    time.appendChild(type);
    time.appendChild(brand);
    time.appendChild(price);
    time.appendChild(num);
    time.appendChild(sum);
}


// 显示日销售清单
void Widget::showDailyList()
{
    ui->dailyList->clear();
    if (docRead()) {
        QDomElement root = doc.documentElement();
        QString title = root.tagName();
        QListWidgetItem *titleItem = new QListWidgetItem; 
        titleItem->setText(QString("-----%1-----").arg(title));
        titleItem->setTextAlignment(Qt::AlignCenter);
        ui->dailyList->addItem(titleItem);

        if (root.hasChildNodes()) {
            QString currentDate = getDateTime(Date);
            QDomElement dateElement = root.lastChild().toElement();
            QString date = dateElement.attribute("date");
            if (date == currentDate) {
                ui->dailyList->addItem("");
                ui->dailyList->addItem(QString("日期：%1").arg(date));
                ui->dailyList->addItem("");
                QDomNodeList children = dateElement.childNodes();
                // 遍历当日销售的所有数据
                for (int i=0; i<children.count(); i++) {
                    QDomNode node = children.at(i);
                    QString time = node.toElement().attribute("time");
                    QDomNodeList list = node.childNodes();
                    QString type = list.at(0).toElement().text();
                    QString brand = list.at(1).toElement().text();
                    QString price = list.at(2).toElement().text();
                    QString num = list.at(3).toElement().text();
                    QString sum = list.at(4).toElement().text();

                    QString str = time + " 出售 " + brand + type
                            + " " + num + "份， " + "范围：" + price
                            + "， 描述：" + sum ;
                    QListWidgetItem *tempItem = new QListWidgetItem;
                    tempItem->setText("**************************");
                    tempItem->setTextAlignment(Qt::AlignCenter);
                    ui->dailyList->addItem(tempItem);
                    ui->dailyList->addItem(str);
                }
            }
        }
    }
}


// 创建销售记录图表的模型和视图
void Widget::createChartModelView()
{
    chartModel = new QStandardItemModel(this);
    chartModel->setColumnCount(2);
    chartModel->setHeaderData(0, Qt::Horizontal, QString("地名"));
    chartModel->setHeaderData(1, Qt::Horizontal, QString("销售数量"));

    QSplitter *splitter = new QSplitter(ui->chartPage);
    splitter->resize(700, 320);
    splitter->move(0, 80);
    QTableView *table = new QTableView;
    PieView *pieChart = new PieView;
    splitter->addWidget(table);
    splitter->addWidget(pieChart);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    table->setModel(chartModel);
    pieChart->setModel(chartModel);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(chartModel);
    table->setSelectionModel(selectionModel);
    pieChart->setSelectionModel(selectionModel);
}

// 显示销售记录图表
void Widget::showChart()
{
    QString type = ui->typeComboBox->currentText();
    if (type==QString::fromLocal8Bit("数据类型"))
    {
        return;
    }
    QSqlQuery query;
    query.exec(QString("select name,allselled from metadata where type=(select id from type where name='%1' )")
               .arg(type));

    chartModel->removeRows(0, chartModel->rowCount(QModelIndex()), QModelIndex());

    int row = 0;

    while(query.next()) {
        int r = qrand() % 256;
        int g = qrand() % 256;
        int b = qrand() % 256;

        chartModel->insertRows(row, 1, QModelIndex());

        chartModel->setData(chartModel->index(row, 0, QModelIndex()),
                            query.value(0).toString());
        chartModel->setData(chartModel->index(row, 1, QModelIndex()),
                            query.value(1).toInt());
        chartModel->setData(chartModel->index(row, 0, QModelIndex()),
                            QColor(r, g, b), Qt::DecorationRole);
        row++;
    }
}

// 销售统计页面的类型选择框
void Widget::on_typeComboBox_currentIndexChanged(QString type)
{
    if (type != "请选择类型")
        showChart();
}

// 更新显示按钮
void Widget::on_updateBtn_clicked()
{
    if (ui->typeComboBox->currentText() != "请选择类型")
        showChart();
}

// 地理数据管理按钮
void Widget::on_manageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

// 地理数据销售统计按钮
void Widget::on_chartBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// 修改密码按钮
void Widget::on_passwordBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

// 修改密码确定按钮
void Widget::on_changePwdBtn_clicked()
{
    if (ui->oldPwdLineEdit->text().isEmpty() ||
            ui->newPwdLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请将信息填写完整！"),
                             QMessageBox::Ok);
    } else {
        QSqlQuery query;
        query.exec("select password from user");
        query.next();
        if (query.value(0).toString() == ui->oldPwdLineEdit->text()) {
            bool temp = query.exec(QString("update user set password='%1' where password='%2'")
                                   .arg(ui->newPwdLineEdit->text()).arg(ui->oldPwdLineEdit->text()));
            if (temp) {
                QMessageBox::information(this, tr("提示"), tr("密码修改成功！"),
                                         QMessageBox::Ok);
                ui->oldPwdLineEdit->clear();
                ui->newPwdLineEdit->clear();
            } else {
                QMessageBox::information(this, tr("提示"), tr("密码修改失败，无法访问数据库！"),
                                         QMessageBox::Ok);
            }
        } else {
            QMessageBox::warning(this, tr("警告"), tr("原密码错误，请重新填写！"),
                                 QMessageBox::Ok);
            ui->oldPwdLineEdit->clear();
            ui->newPwdLineEdit->clear();
            ui->oldPwdLineEdit->setFocus();
        }
    }

}


























