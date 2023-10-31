#include <QObject>
#include <QHeaderView>
#include <QUrl>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include "htcore/Core.h"
 
class HCommonHeaderView : public QHeaderView
{
    Q_OBJECT
    Q_PROPERTY(int textAlign READ getTextAlign WRITE setTextAlign )  //!表头文本对齐方式
    Q_PROPERTY(QColor bgColor MEMBER m_gbColor)                      //!section背景颜色
    Q_PROPERTY(QColor borderColor MEMBER m_borderColor )             //!边框颜色*/
    Q_PROPERTY(int leftMargin MEMBER m_iLeftMargin )                 //!section做边距
    Q_PROPERTY(QUrl ascendingOrderUrl MEMBER  m_ascendingOrderUrl)   //!递增图标
    Q_PROPERTY(QUrl descendingOrderUrl MEMBER  m_descendingOrderUrl) //!递减图标
public:
    explicit HCommonHeaderView(Qt::Orientation orientation = Qt::Horizontal,QWidget *parent = 0);
 
    void restoreDefaultState();
 
    void setHeaderText(const QStringList &list);
    void setHeaderText_NoSort(const QStringList &list);
    void setCurSortColumn(int column);
    int getCurSortIndex() const;
 
    void setTextAlign(int flag);       //设置表头列文字位置
    int getTextAlign() const;          //获取表头列文字位置
    QUrl getAscendingOrderUrl() const; //获取递增图标url
    QUrl getDescendingOrderUrl() const;//获取递减图标url
 
    ///
    /// \brief setCheckBoxForColumn
    /// \param column
    ///  设置检查框对应的列，不设置则没有检查框
    ///
    void setCheckBoxForColumn(int column);
 
    ///
    /// \brief getCheckState
    /// \return
    ///  得到当前检查框状态
    ///
    Qt::CheckState getCheckState() const;
 
    //!
    //!设置整全部行数的对齐方式
    //! flag:true(设置全部列数);flase:(设置model数据的对齐方式)
    void setAllColumnAligFlag(bool flag);
    bool getBSortFlag() const;
    void setBSortFlag(bool value);
 
    void setShowBottonLine(bool value);
 
signals:
    //!
    //! \brief check状态改变
    //!
    void signalCheckStateChanged(Qt::CheckState);
    //!
    //! \brief signalSortStateChanged 排序信号
    //! \param logicalIndex           列序号
    //!
    void signalSortStateChanged(int logicalIndex);
 
public slots:
    //!
    //! \brief slotCheckStateChanged  改变check状态
    //! \param state  check状态
    //!
    void slotCheckStateChanged(Qt::CheckState state);
 
protected:
    ///
    /// \brief paintSection
    /// \param painter
    /// \param rect
    /// \param logicalIndex
    ///  绘制表头
    ///
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
 
    ///
    /// \brief mousePressEvent
    /// \param e
    /// 模拟鼠标点击
    ///
    void mousePressEvent(QMouseEvent *e);
 
    ///
    /// \brief mouseReleaseEvent
    /// \param e
    ///  检查鼠标释放位置是否合理，合理则处理鼠标点击，反之，不处理。
    ///
    void mouseReleaseEvent(QMouseEvent *e);
 
private:
    void setCheckState(Qt::CheckState state);
 
    ///
    /// \brief paintSortIndicatorIcon
    /// \param painter
    /// \param rect
    /// 绘制排序图标
    ///
    void paintSortIndicatorIcon(QPainter *painter, const QRect &rect, int logicIndex) const;
 
    ///
    /// \brief paintCheckBoxIcon
    /// \param painter
    /// \param rect
    /// 绘制组合框图标
    ///
    void paintCheckBoxIcon(QPainter *painter, const QRect &rect) const;
 
    ///
    /// \brief paintSectionRightBorderLine
    /// \param painter
    /// \param rect
    /// 绘制表头section右边界线
    ///
    void paintSectionRightBorderLine(QPainter *painter, const QRect &rect) const;
 
    void paintSectionText(QPainter *painter, const QRect &rect, int logicIndex) const;
 
 
 
private:
    QVector<int> m_sortColumnLst;  //存储能够进行排序的索引
    QStringList m_headerLst;
 
    Qt::CheckState m_checkBoxState;
    int m_iSortIndex;
 
    bool m_bMousePressed;
    bool m_bSectionSizeChanging;
    int m_bPressedIndex;
 
    int m_iTextAlign;   //文本对齐方式
    int m_iLeftMargin;  //左边距
    QColor m_gbColor;  //背景色
    QColor m_borderColor; //边框颜色
    int m_iCheckBoxColumn; //表头列，对应的检查框
    bool m_bIsFirst;
    QUrl m_ascendingOrderUrl;
    QUrl m_descendingOrderUrl;
    bool bAllColumnAlgFlag{true}; //设置全部行列的对齐方式标志
    bool bSortFlag;
    bool mbShowBottonLine{false};
 
};