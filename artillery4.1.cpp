#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GRAVITY 9.81

typedef struct {
    char name[50];
    double initial_velocity;
    double head_mass;
    double head_drag_coefficient;
    double head_cross_section_area;
    double explosion_radius;
} Ammunition;

Ammunition* ammunition_list = NULL;
int num_ammunition = 0;

void displayMainMenu();
void shoot();
void addAmmunition();
void viewAmmunition();
void deleteAmmunition();
void saveAmmunition();
void loadAmmunition();
void viewShootingLog();

int main() {
    loadAmmunition(); // ���ļ����ص�ҩ��Ϣ

    int choice;

    while (1) {
        displayMainMenu();

        printf("����������ѡ��: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                shoot();
                break;
            case 2:
                addAmmunition();
                break;
            case 3:
                viewAmmunition();
                break;
            case 4:
                deleteAmmunition();
                break;
            case 5:
                viewShootingLog();
                break;
            default:
                printf("��Ч��ѡ�������ԡ�\n");
                break;
        }
    }

    return 0;
}

void displayMainMenu() {
    printf("\n--- ���˵� ---\n");
    printf("1. ���\n");
    printf("2. ��ӵ�ҩ\n");
    printf("3. �鿴��ҩ\n");
    printf("4. ɾ����ҩ\n");
    printf("5. �����־\n");
}

void shoot() {
    if (num_ammunition == 0) {
        printf("û�п��õĵ�ҩ��\n");
        return;
    }

    int ammo_choice;
    printf("\n--- ���õ�ҩ ---\n");
    for (int i = 0; i < num_ammunition; i++) {
        printf("%d. %s\n", i + 1, ammunition_list[i].name);
    }
    printf("��ѡ��ҩ: ");
    scanf("%d", &ammo_choice);

    if (ammo_choice < 1 || ammo_choice > num_ammunition) {
        printf("��Ч�ĵ�ҩѡ��\n");
        return;
    }

    double target_x, target_y;
    printf("������Ŀ�����꣨x, y��: ");
    scanf("%lf %lf", &target_x, &target_y);

    Ammunition ammo = ammunition_list[ammo_choice - 1];

    double max_range = (pow(ammo.initial_velocity, 2) * sin(2 * 45 * M_PI / 180.0)) / GRAVITY;

    if (target_x > max_range) {
        printf("Ŀ�곬����̣�\n");
        printf("������: %.2lf\n", max_range);
        return;
    }

    double angle = 0.0;
    double lower = 0.0;
    double upper = 90.0;
    double epsilon = 0.01;

    while (upper - lower > epsilon) {
        angle = (lower + upper) / 2.0;
        double time_of_flight = (2 * ammo.initial_velocity * sin(angle * M_PI / 180.0)) / GRAVITY;
        double velocity_x = ammo.initial_velocity * cos(angle * M_PI / 180.0);
        double velocity_y = ammo.initial_velocity * sin(angle * M_PI / 180.0);

        double x = 0.0;
        double y = 0.0;

        double air_density = 1.29;
        double air_pressure = 101325.0;
        double pressure_gradient = 3.0;

        double drag_force = 0.0;
        double drag_coefficient = ammo.head_drag_coefficient;
        double cross_section_area = ammo.head_cross_section_area;

        double time_step = 0.001;
        double time = 0.0;

        while (y >= 0.0) {
            drag_force = 0.5 * air_density * velocity_y * velocity_y * drag_coefficient * cross_section_area;
            double drag_acceleration = drag_force / ammo.head_mass;

            double pressure_drop = pressure_gradient * y;

            double acceleration_x = -drag_acceleration * velocity_x / ammo.initial_velocity;
            double acceleration_y = -GRAVITY - drag_acceleration * velocity_y / ammo.initial_velocity - pressure_drop / ammo.head_mass;

            velocity_x += acceleration_x * time_step;
            velocity_y += acceleration_y * time_step;

            x += velocity_x * time_step;
            y += velocity_y * time_step;

            time += time_step;
        }

        double horizontal_distance = x;
        if (horizontal_distance > target_x) {
            upper = angle;
        } else {
            lower = angle;
        }
    }

    printf("Ԥ������Ƕ�: %.2lf ��\n", angle);

    // ��¼�����־
    FILE* log_file = fopen("shootinglog.txt", "a");
    if (log_file == NULL) {
        printf("�޷��������־�ļ���\n");
        return;
    }

    fprintf(log_file, "��ҩ����: %s\n", ammo.name);
    fprintf(log_file, "����Ƕ�: %.2lf ��\n", angle);
    fprintf(log_file, "Ŀ������: (%.2lf, %.2lf)\n", target_x, target_y);
    fprintf(log_file, "--------------------------\n");

    fclose(log_file);
}


void addAmmunition() {
    Ammunition ammo;
    printf("\n���뵯ҩ����:\n");
    printf("����������: ");
    scanf("%s", ammo.name);

    printf("�������ʼ�ٶ�: ");
    scanf("%lf", &ammo.initial_velocity);

    printf("�����뵯ͷ����: ");
    scanf("%lf", &ammo.head_mass);

    printf("�����뵯ͷ����ϵ��: ");
    scanf("%lf", &ammo.head_drag_coefficient);

    printf("�����뵯ͷ�����: ");
    scanf("%lf", &ammo.head_cross_section_area);

    printf("�����뱬ը�뾶: ");
    scanf("%lf", &ammo.explosion_radius);

    ammunition_list = (Ammunition*)realloc(ammunition_list, (num_ammunition + 1) * sizeof(Ammunition));
    ammunition_list[num_ammunition] = ammo;
    num_ammunition++;

    printf("��ҩ��ӳɹ���\n");

    saveAmmunition(); // ���浯ҩ��Ϣ���ļ�
}

void viewAmmunition() {
    if (num_ammunition == 0) {
        printf("û�п��õĵ�ҩ��\n");
        return;
    }

    printf("\n--- ��ҩ�б� ---\n");
    for (int i = 0; i < num_ammunition; i++) {
        printf("��ҩ����: %s\n", ammunition_list[i].name);
        printf("��ʼ�ٶ�: %.2lf\n", ammunition_list[i].initial_velocity);
        printf("��ͷ����: %.2lf\n", ammunition_list[i].head_mass);
        printf("��ͷ����ϵ��: %.2lf\n", ammunition_list[i].head_drag_coefficient);
        printf("��ͷ�����: %.2lf\n", ammunition_list[i].head_cross_section_area);
        printf("��ը�뾶: %.2lf\n", ammunition_list[i].explosion_radius);
        printf("--------------------------\n");
    }
}

void deleteAmmunition() {
    if (num_ammunition == 0) {
        printf("û�п��õĵ�ҩ��\n");
        return;
    }

    int ammo_choice;
    printf("\n--- ���õ�ҩ ---\n");
    for (int i = 0; i < num_ammunition; i++) {
        printf("%d. %s\n", i + 1, ammunition_list[i].name);
    }
    printf("��ѡ��Ҫɾ���ĵ�ҩ: ");
    scanf("%d", &ammo_choice);

    if (ammo_choice < 1 || ammo_choice > num_ammunition) {
        printf("��Ч�ĵ�ҩѡ��\n");
        return;
    }

    for (int i = ammo_choice - 1; i < num_ammunition - 1; i++) {
        ammunition_list[i] = ammunition_list[i + 1];
    }

    num_ammunition--;
    ammunition_list = (Ammunition*)realloc(ammunition_list, num_ammunition * sizeof(Ammunition));

    printf("��ҩɾ���ɹ���\n");

    saveAmmunition(); // ���浯ҩ��Ϣ���ļ�
}

void saveAmmunition() {
    FILE* file = fopen("ammunition.txt", "w");
    if (file == NULL) {
        printf("�޷��򿪵�ҩ�ļ���\n");
        return;
    }

    fprintf(file, "%d\n", num_ammunition);
    for (int i = 0; i < num_ammunition; i++) {
        fprintf(file, "%s\n", ammunition_list[i].name);
        fprintf(file, "%.2lf\n", ammunition_list[i].initial_velocity);
        fprintf(file, "%.2lf\n", ammunition_list[i].head_mass);
        fprintf(file, "%.2lf\n", ammunition_list[i].head_drag_coefficient);
        fprintf(file, "%.2lf\n", ammunition_list[i].head_cross_section_area);
        fprintf(file, "%.2lf\n", ammunition_list[i].explosion_radius);
    }

    fclose(file);
}

void loadAmmunition() {
    FILE* file = fopen("ammunition.txt", "r");
    if (file == NULL) {
        printf("�޷����ص�ҩ�ļ���\n");
        return;
    }

    fscanf(file, "%d\n", &num_ammunition);
    ammunition_list = (Ammunition*)malloc(num_ammunition * sizeof(Ammunition));

    for (int i = 0; i < num_ammunition; i++) {
        fscanf(file, "%s\n", ammunition_list[i].name);
        fscanf(file, "%lf\n", &ammunition_list[i].initial_velocity);
        fscanf(file, "%lf\n", &ammunition_list[i].head_mass);
        fscanf(file, "%lf\n", &ammunition_list[i].head_drag_coefficient);
        fscanf(file, "%lf\n", &ammunition_list[i].head_cross_section_area);
        fscanf(file, "%lf\n", &ammunition_list[i].explosion_radius);
    }

    fclose(file);
}

void viewShootingLog() {
    FILE* file = fopen("shootinglog.txt", "r");
    if (file == NULL) {
        printf("�޷��������־�ļ���\n");
        return;
    }

    printf("\n--- �����־ ---\n");

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
}

