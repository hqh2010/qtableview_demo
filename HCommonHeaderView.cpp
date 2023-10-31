#include "HCommonHeaderView.h"
#include <QRgba64>
#define ICON_SIZE 12          //图标大小
#define ICON_RIGHT_MARGIN 6   //离右边缘间距
#define HEADER_HEIGHT 18      //表头高
 
 
HCommonHeaderView::HCommonHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation,parent)
    ,m_checkBoxState(Qt::Unchecked)
    ,m_bMousePressed(false)
    ,m_bSectionSizeChanging(false)
    ,m_iCheckBoxColumn(999)
    ,m_bPressedIndex(999)
    ,m_iSortIndex(999)
    ,m_bIsFirst(true)
    ,bSortFlag(true)
{
    this->setSortIndicatorShown(false);
    setTextAlign(0);
    m_gbColor = QColor("#282D35");
    this->setSectionResizeMode(QHeaderView::Interactive);
 
    for(int i = 0; i < 20; ++i)
    {
        m_headerLst.append("");
    }
    connect(this,&HCommonHeaderView::sectionResized,[&](){ // 列拖动改变大小
        m_bSectionSizeChanging = true;
    });
}
 
void HCommonHeaderView::restoreDefaultState()
{
    m_checkBoxState = Qt::Unchecked;
    this->viewport()->update();
}
 
void HCommonHeaderView::setHeaderText(const QStringList &list)
{
    m_headerLst.clear();
    m_headerLst = list;
    for(int i = 0; i < list.count() - 1; i++)
    {
        m_sortColumnLst.append(i);
    }
}
 
void HCommonHeaderView::setCurSortColumn(int column)
{
    m_iSortIndex = column;
    this->viewport()->update();
}
 
int HCommonHeaderView::getCurSortIndex() const
{
    return m_iSortIndex;
}
 
void HCommonHeaderView::setTextAlign(int flag)
{
    m_iTextAlign = flag;
}
 
int HCommonHeaderView::getTextAlign() const
{
    return m_iTextAlign;
}
 
QUrl HCommonHeaderView::getAscendingOrderUrl() const
{
    return m_ascendingOrderUrl;
}
 
QUrl HCommonHeaderView::getDescendingOrderUrl() const
{
    return m_descendingOrderUrl;
}
 
void HCommonHeaderView::setCheckBoxForColumn(int column)
{
    m_iCheckBoxColumn = column;
}
 
void HCommonHeaderView::slotCheckStateChanged(Qt::CheckState state)
{
    if(state != m_checkBoxState) {
        m_checkBoxState = state;
        this->viewport()->update();
    }
}
 
void HCommonHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    QPen pen(QColor(200,200,200,255));
    painter->setPen(pen);
    QFont font = painter->font();
    font.setPixelSize(12);
    painter->setFont(font);
    paintSectionText(painter,rect,logicalIndex);
 
    if(logicalIndex == m_iCheckBoxColumn)
    {
        paintCheckBoxIcon(painter,rect);
    }
    else if( m_iSortIndex == logicalIndex && bSortFlag)
    {
        paintSortIndicatorIcon(painter,rect,logicalIndex);
    }
    if(logicalIndex == m_iCheckBoxColumn)
    {
        return;
    }
    if(logicalIndex == m_headerLst.count() - 1) return;
    paintSectionRightBorderLine(painter,rect);
}
 
void HCommonHeaderView::mousePressEvent(QMouseEvent *e)
{
    int column = logicalIndexAt(e->pos());
    if(column >= 0 ) {
        m_bMousePressed = true;
        m_bPressedIndex = column;
    }
    QHeaderView::mousePressEvent(e);
}
 
void HCommonHeaderView::mouseReleaseEvent(QMouseEvent *e)
{
    int column = logicalIndexAt(e->pos());
    //    if(m_bIsFirst) {
    //        m_bSectionSizeChanging = false;
    //        m_bIsFirst = false;
    //    }
    if( column >= 0 && !m_bSectionSizeChanging)
    {
        if(column == m_iCheckBoxColumn && m_bPressedIndex == column) //检查框被点击
        {
            this->setCheckState( (m_checkBoxState == Qt::Checked) ? Qt::Unchecked : Qt::Checked );
        }
        else if(column != m_iCheckBoxColumn)
        {
            m_iSortIndex = column;
            emit signalSortStateChanged(m_iSortIndex);
        }
        this->viewport()->update();
    }
    m_bMousePressed = false;
    m_bSectionSizeChanging = false;
    QHeaderView::mouseReleaseEvent(e);
}
 
void HCommonHeaderView::setCheckState(Qt::CheckState state)
{
    m_checkBoxState = state;
    emit signalCheckStateChanged(state);
}
 
Qt::CheckState HCommonHeaderView::getCheckState() const
{
    return m_checkBoxState;
}
 
void HCommonHeaderView::paintSortIndicatorIcon(QPainter *painter, const QRect &rect,int logicIndex) const
{
    if(m_headerLst.at(logicIndex) == "" && !m_sortColumnLst.contains(logicIndex) )
    {
        return;
    }
    int y1 = rect.y() + (rect.height() - HEADER_HEIGHT) / 2;
 
    QRect tmpRect ;
    tmpRect.setY(y1);
    tmpRect.setX(rect.x());
    tmpRect.setHeight(HEADER_HEIGHT);
    tmpRect.setWidth(rect.width());
 
    if(m_iTextAlign == 1)
    {
        tmpRect = tmpRect.adjusted(m_iLeftMargin,0,m_iLeftMargin,0);
    }
 
    QFontMetrics metrics = painter->fontMetrics();
    QRect textRect = metrics.boundingRect(tmpRect,m_iTextAlign|Qt::AlignVCenter,m_headerLst.at(logicIndex));
 
    QUrl iconUrl  =m_descendingOrderUrl;
    if(this->sortIndicatorOrder() == Qt::AscendingOrder)
    {
        iconUrl  = m_ascendingOrderUrl;
    }
 
    QPixmap tmp(QString("HTSkin:%1/%2").arg(HCore::instance()->getCurrentSkinId())
                .arg(iconUrl.path()));
 
    QRect iconRect = textRect.adjusted(ICON_RIGHT_MARGIN + textRect.width(),0,ICON_RIGHT_MARGIN + textRect.width(),0);
    QApplication::style()->drawItemPixmap(painter,iconRect,Qt::AlignLeft|Qt::AlignVCenter,tmp);
}
 
void HCommonHeaderView::paintCheckBoxIcon(QPainter *painter, const QRect &rect) const
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    //    QUrl iconUrl;
    //    switch(m_checkBoxState) {
    //        case Qt::Unchecked: {
    //            iconUrl = BD_GL_ALGORITHMIC_COMMON_STYLE->getUncheckUrl();
    //        } break;
    //        case Qt::PartiallyChecked: {
    //            iconUrl = BD_GL_ALGORITHMIC_COMMON_STYLE->getPartCheckUrl();
    //        } break;
    //        case Qt::Checked: {
    //            iconUrl = BD_GL_ALGORITHMIC_COMMON_STYLE->getCheckUrl();
    //        } break;
    //    }
    //    QPixmap tmp(QString("BDSkin:%1/%2").arg(gBDCore->getCurrentSkinId())
    //                .arg(iconUrl.path()));
 
    //    QApplication::style()->drawItemPixmap(painter,rect.adjusted(0,0,-6,0),Qt::AlignVCenter|Qt::AlignRight,tmp);
}
 
void HCommonHeaderView::paintSectionRightBorderLine(QPainter *painter, const QRect &rect) const
{
    int x1 = rect.right();
    int y1 = rect.y() + (rect.height() - HEADER_HEIGHT) / 2;
 
    int x2 = x1;
    int y2 = y1 + HEADER_HEIGHT;
 
    //! [0]
    painter->save();
    //! [1]
    QPen pen;
    pen.setColor(m_borderColor);
    pen.setWidth(2);
    painter->setPen(pen);
    //! [2]
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(QPoint(x1,y1),QPoint(x2,y2));
 
    if(mbShowBottonLine){
        painter->drawLine(QPoint(rect.left(),rect.height()),QPoint(rect.right(),rect.height()));
    }
    //! [3]
    painter->restore();
}
 
void HCommonHeaderView::paintSectionText(QPainter *painter, const QRect &rect, int logicIndex) const
{
    int y1 = rect.y() + (rect.height() - HEADER_HEIGHT) / 2 -3;
 
    QRect tmpRect ;
    tmpRect.setY(y1);
    tmpRect.setX(rect.x());
    tmpRect.setHeight(HEADER_HEIGHT);
    tmpRect.setWidth(rect.width());
 
 
    painter->fillRect(rect,m_gbColor);
    if( m_iCheckBoxColumn == logicIndex ) return;
    painter->save();
    if(bAllColumnAlgFlag ){
        if(m_iTextAlign == 1) {//左对齐
            painter->drawText(tmpRect.adjusted(m_iLeftMargin,0,m_iLeftMargin,0),m_iTextAlign|Qt::AlignVCenter,m_headerLst.at(logicIndex));
        }else{
            painter->drawText(tmpRect,m_iTextAlign|Qt::AlignVCenter,m_headerLst.at(logicIndex));
        }
    }else{  //这个是根据model数据方式对齐
        tmpRect.setX(rect.x()+10);
        tmpRect.setWidth(tmpRect.width()-10);
        int nAlignment = this->model()->data(this->model()->index(0,logicIndex),Qt::TextAlignmentRole).toInt();
        painter->drawText(tmpRect,nAlignment,m_headerLst.at(logicIndex));
    }
 
    painter->restore();
 
}
 
 
 
bool HCommonHeaderView::getBSortFlag() const
{
    return bSortFlag;
}
 
void HCommonHeaderView::setBSortFlag(bool value)
{
    bSortFlag = value;
}
 
void HCommonHeaderView::setShowBottonLine(bool value)
{
    mbShowBottonLine = value;
}
 
void HCommonHeaderView::setAllColumnAligFlag(bool flag)
{
    bAllColumnAlgFlag = flag;
}
 
void HCommonHeaderView::setHeaderText_NoSort(const QStringList &list)
{
    m_headerLst.clear();
    m_headerLst = list;
}
 