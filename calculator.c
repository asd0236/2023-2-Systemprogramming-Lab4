#include<stdlib.h>
#include<stdio.h>
#include <gtk/gtk.h>

static GtkWidget *entry;

// 버튼 클릭 시 호출되는 콜백 함수
void button_clicked(GtkButton *button, gpointer data) {
    const gchar *button_label = gtk_button_get_label(button);
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));

    // 현재 입력된 텍스트와 버튼의 레이블을 합쳐서 업데이트
    gchar *new_text = g_strdup_printf("%s%s", current_text, button_label);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}

// 계산기 생성 함수
GtkWidget *create_calculator() {
    GtkWidget *window, *grid, *button;
    gchar *button_labels[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "=", "+"
    };

    // 윈도우 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // 그리드 생성
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 텍스트 입력창 생성
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // 버튼 생성 및 이벤트 핸들러 연결
    for (int i = 0; i < 16; ++i) {
        button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, i / 4 + 1, 1, 1);
    }

    // 윈도우 종료 시 프로그램 종료
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = create_calculator();

    // 윈도우와 그 안의 위젯들을 화면에 표시
    gtk_widget_show_all(window);

    // GTK 이벤트 루프 시작
    gtk_main();

    return 0;
}
