#include "qt.cc"
#include "QoreQtDynamicMethod.cc"
#include "QC_QObject.cc"
#include "QC_QApplication.cc"
#include "QC_QPushButton.cc"
#include "QC_QFont.cc"
#include "QC_QMatrix.cc"
#include "QC_QWidget.cc"
#include "QC_QFrame.cc"
#include "QC_QLCDNumber.cc"
#include "QC_QAbstractSlider.cc"
#include "QC_QSlider.cc"
#include "QC_QLayout.cc"
#include "QC_QBoxLayout.cc"
#include "QC_QVBoxLayout.cc"
#include "QC_QHBoxLayout.cc"
#include "QC_QGridLayout.cc"
#include "QC_QBrush.cc"
#include "QC_QColor.cc"
#include "QC_QPaintDevice.cc"
#include "QC_QRect.cc"
#include "QC_QRectF.cc"
#include "QC_QPainter.cc"
#include "QC_QPalette.cc"
#include "QC_QRegion.cc"
#include "QC_QPoint.cc"
#include "QC_QSize.cc"
#include "QC_QTimer.cc"
#include "QC_QLabel.cc"
#include "QC_QPicture.cc"
#include "QC_QPixmap.cc"
#include "QC_QMovie.cc"
#include "QC_QBitmap.cc"
#include "QC_QImage.cc"
#include "QC_QDateTime.cc"
#include "QC_QDate.cc"
#include "QC_QTime.cc"
#include "QC_QIcon.cc"
#include "QC_QKeySequence.cc"
#include "QC_QAction.cc"
#include "QC_QActionGroup.cc"
#include "QC_QShortcut.cc"

#include "QT_BrushStyle.cc"
#include "QT_PenStyle.cc"

#include "QC_QEvent.cc"
#include "QC_QInputEvent.cc"
#include "QC_QPaintEvent.cc"
#include "QC_QMouseEvent.cc"
#include "QC_QKeyEvent.cc"
#include "QC_QMoveEvent.cc"
#include "QC_QResizeEvent.cc"
#include "QC_QPointF.cc"
#include "QC_QPolygon.cc"
#include "QC_QPolygonF.cc"
#include "QC_QLine.cc"
#include "QC_QLineF.cc"
#include "QC_QAbstractButton.cc"
#include "QC_QMenu.cc"
#include "QC_QToolButton.cc"
#include "QC_QDialog.cc"
#include "QC_QLineEdit.cc"
#include "QC_QTextLength.cc"
#include "QC_QTextFormat.cc"
#include "QC_QTextBlockFormat.cc"
#include "QC_QTextCharFormat.cc"
#include "QC_QPen.cc"
#include "QC_QTextFrameFormat.cc"
#include "QC_QTextTableFormat.cc"
#include "QC_QTextListFormat.cc"
#include "QC_QTextImageFormat.cc"
#include "QC_QCalendarWidget.cc"
#include "QC_QStyleOption.cc"
#include "QC_QModelIndex.cc"
#include "QC_QStyleOptionViewItem.cc"
#include "QC_QStyleOptionViewItemV2.cc"
#include "QC_QAbstractItemModel.cc"
#include "QC_QAbstractItemDelegate.cc"
#include "QC_QItemDelegate.cc"
#include "QC_QComboBox.cc"
#include "QC_QCheckBox.cc"
#include "QC_QAbstractSpinBox.cc"
#include "QC_QDateTimeEdit.cc"
#include "QC_QLocale.cc"
#include "QC_QByteArray.cc"
#include "QC_QUrl.cc"
#include "QC_QVariant.cc"
#include "QC_QGroupBox.cc"
#include "QC_QDateEdit.cc"
#include "QC_QFontMetrics.cc"
#include "QC_QFontDatabase.cc"
#include "QC_QFontInfo.cc"
#include "QC_QScrollBar.cc"
#include "QC_QAbstractScrollArea.cc"
#include "QC_QScrollArea.cc"
#include "QC_QChar.cc"
#include "QC_QActionEvent.cc"
#include "QC_QCloseEvent.cc"
#include "QC_QContextMenuEvent.cc"
#include "QC_QDropEvent.cc"
#include "QC_QMimeData.cc"
#include "QC_QDragMoveEvent.cc"
#include "QC_QDragEnterEvent.cc"
#include "QC_QDragLeaveEvent.cc"
#include "QC_QFocusEvent.cc"
#include "QC_QHideEvent.cc"
#include "QC_QInputMethodEvent.cc"
#include "QC_QShowEvent.cc"
#include "QC_QTabletEvent.cc"
#include "QC_QWheelEvent.cc"
#include "QC_QClipboard.cc"
#include "QC_QFontComboBox.cc"
#include "QC_QMainWindow.cc"
#include "QC_QRadioButton.cc"
#include "QC_QStyle.cc"
#include "QC_QStyleOptionComplex.cc"
#include "QC_QStyleOptionComboBox.cc"
#include "QC_QStyleOptionGroupBox.cc"
#include "QC_QStyleOptionSizeGrip.cc"
#include "QC_QStyleOptionSlider.cc"
#include "QC_QStyleOptionSpinBox.cc"
#include "QC_QStyleOptionTitleBar.cc"
#include "QC_QStyleOptionToolButton.cc"
#include "QC_QMotifStyle.cc"
#include "QC_QCDEStyle.cc"
#include "QC_QWindowsStyle.cc"
#include "QC_QCleanlooksStyle.cc"
#ifdef DARWIN
#include "QC_QMacStyle.cc"
#endif
#include "QC_QPlastiqueStyle.cc"
#ifdef WINDOWS
#include "QC_QWindowsXPStyle.cc"
#endif
#include "QC_QSpinBox.cc"
#include "QC_QAbstractItemView.cc"
#include "QC_QTableView.cc"
#include "QC_QTableWidget.cc"
#include "QC_QTableWidgetItem.cc"
#include "QC_QStyleOptionMenuItem.cc"
#include "QC_QMessageBox.cc"
#include "QC_QStyleOptionButton.cc"
#include "QC_QFileDialog.cc"
#include "QC_QDir.cc"
#include "QC_QHeaderView.cc"
#include "QC_QMetaObject.cc"
#include "QC_QMenuBar.cc"
#include "QC_QPrinter.cc"
#include "QC_QPrintDialog.cc"
#include "QC_QRegExp.cc"
#include "QC_QValidator.cc"
#include "QC_QDoubleValidator.cc"
#include "QC_QIntValidator.cc"
#include "QC_QRegExpValidator.cc"
#include "QC_QFileInfo.cc"
#include "QC_QColorDialog.cc"
#include "QC_QInputDialog.cc"
#include "QC_QIODevice.cc"
#include "QC_QImageWriter.cc"
#include "QC_QDial.cc"
#include "QC_QStackedWidget.cc"
#include "QC_QDoubleSpinBox.cc"
#include "QC_QTimeEdit.cc"
#include "QC_QProgressBar.cc"
#include "QC_QPainterPath.cc"
#include "QC_QPaintEngine.cc"
#include "QC_QBasicTimer.cc"
#include "QC_QTextEdit.cc"
#include "QC_QTabBar.cc"
#include "QC_QStyleOptionTab.cc"
#include "QC_QStyleOptionTabWidgetFrame.cc"
#include "QC_QTabWidget.cc"
#include "QC_QChildEvent.cc"
#include "QC_QTimerEvent.cc"
