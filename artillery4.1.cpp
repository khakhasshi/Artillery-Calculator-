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
    loadAmmunition(); // 从文件加载弹药信息

    int choice;

    while (1) {
        displayMainMenu();

        printf("请输入您的选择: ");
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
                printf("无效的选择！请重试。\n");
                break;
        }
    }

    return 0;
}

void displayMainMenu() {
    printf("\n--- 主菜单 ---\n");
    printf("1. 射击\n");
    printf("2. 添加弹药\n");
    printf("3. 查看弹药\n");
    printf("4. 删除弹药\n");
    printf("5. 射击日志\n");
}

void shoot() {
    if (num_ammunition == 0) {
        printf("没有可用的弹药！\n");
        return;
    }

    int ammo_choice;
    printf("\n--- 可用弹药 ---\n");
    for (int i = 0; i < num_ammunition; i++) {
        printf("%d. %s\n", i + 1, ammunition_list[i].name);
    }
    printf("请选择弹药: ");
    scanf("%d", &ammo_choice);

    if (ammo_choice < 1 || ammo_choice > num_ammunition) {
        printf("无效的弹药选择！\n");
        return;
    }

    double target_x, target_y;
    printf("请输入目标坐标（x, y）: ");
    scanf("%lf %lf", &target_x, &target_y);

    Ammunition ammo = ammunition_list[ammo_choice - 1];

    double max_range = (pow(ammo.initial_velocity, 2) * sin(2 * 45 * M_PI / 180.0)) / GRAVITY;

    if (target_x > max_range) {
        printf("目标超出射程！\n");
        printf("最大射程: %.2lf\n", max_range);
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

    printf("预计射击角度: %.2lf 度\n", angle);

    // 记录射击日志
    FILE* log_file = fopen("shootinglog.txt", "a");
    if (log_file == NULL) {
        printf("无法打开射击日志文件！\n");
        return;
    }

    fprintf(log_file, "弹药类型: %s\n", ammo.name);
    fprintf(log_file, "发射角度: %.2lf 度\n", angle);
    fprintf(log_file, "目标坐标: (%.2lf, %.2lf)\n", target_x, target_y);
    fprintf(log_file, "--------------------------\n");

    fclose(log_file);
}


void addAmmunition() {
    Ammunition ammo;
    printf("\n输入弹药详情:\n");
    printf("请输入名称: ");
    scanf("%s", ammo.name);

    printf("请输入初始速度: ");
    scanf("%lf", &ammo.initial_velocity);

    printf("请输入弹头质量: ");
    scanf("%lf", &ammo.head_mass);

    printf("请输入弹头阻力系数: ");
    scanf("%lf", &ammo.head_drag_coefficient);

    printf("请输入弹头截面积: ");
    scanf("%lf", &ammo.head_cross_section_area);

    printf("请输入爆炸半径: ");
    scanf("%lf", &ammo.explosion_radius);

    ammunition_list = (Ammunition*)realloc(ammunition_list, (num_ammunition + 1) * sizeof(Ammunition));
    ammunition_list[num_ammunition] = ammo;
    num_ammunition++;

    printf("弹药添加成功！\n");

    saveAmmunition(); // 保存弹药信息到文件
}

void viewAmmunition() {
    if (num_ammunition == 0) {
        printf("没有可用的弹药！\n");
        return;
    }

    printf("\n--- 弹药列表 ---\n");
    for (int i = 0; i < num_ammunition; i++) {
        printf("弹药类型: %s\n", ammunition_list[i].name);
        printf("初始速度: %.2lf\n", ammunition_list[i].initial_velocity);
        printf("弹头质量: %.2lf\n", ammunition_list[i].head_mass);
        printf("弹头阻力系数: %.2lf\n", ammunition_list[i].head_drag_coefficient);
        printf("弹头截面积: %.2lf\n", ammunition_list[i].head_cross_section_area);
        printf("爆炸半径: %.2lf\n", ammunition_list[i].explosion_radius);
        printf("--------------------------\n");
    }
}

void deleteAmmunition() {
    if (num_ammunition == 0) {
        printf("没有可用的弹药！\n");
        return;
    }

    int ammo_choice;
    printf("\n--- 可用弹药 ---\n");
    for (int i = 0; i < num_ammunition; i++) {
        printf("%d. %s\n", i + 1, ammunition_list[i].name);
    }
    printf("请选择要删除的弹药: ");
    scanf("%d", &ammo_choice);

    if (ammo_choice < 1 || ammo_choice > num_ammunition) {
        printf("无效的弹药选择！\n");
        return;
    }

    for (int i = ammo_choice - 1; i < num_ammunition - 1; i++) {
        ammunition_list[i] = ammunition_list[i + 1];
    }

    num_ammunition--;
    ammunition_list = (Ammunition*)realloc(ammunition_list, num_ammunition * sizeof(Ammunition));

    printf("弹药删除成功！\n");

    saveAmmunition(); // 保存弹药信息到文件
}

void saveAmmunition() {
    FILE* file = fopen("ammunition.txt", "w");
    if (file == NULL) {
        printf("无法打开弹药文件！\n");
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
        printf("无法加载弹药文件！\n");
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
        printf("无法打开射击日志文件！\n");
        return;
    }

    printf("\n--- 射击日志 ---\n");

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
}

