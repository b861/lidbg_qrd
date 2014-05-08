
#include "lidbg.h"
extern int is_key_scan_en;
int button_en;
struct work_struct work_left_button1;
static void work_left_button1_fn(struct work_struct *work)
{
if(is_key_scan_en)
    SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
}

struct work_struct work_right_button1;
static void work_right_button1_fn(struct work_struct *work)
{
if(is_key_scan_en)
    SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);
}
irqreturn_t irq_left_button1(int irq, void *dev_id)
{
    //lidbg("irq_left_button1: %d\n", irq);
    if(!work_pending(&work_left_button1))
        schedule_work(&work_left_button1);
    return IRQ_HANDLED;

}
irqreturn_t irq_left_button2(int irq, void *dev_id)
{
    //lidbg("irq_left_button2: %d\n", irq);
    return IRQ_HANDLED;
}
irqreturn_t irq_right_button1(int irq, void *dev_id)
{
    //lidbg("irq_right_button1: %d\n", irq);
    if(!work_pending(&work_right_button1))
        schedule_work(&work_right_button1);
    return IRQ_HANDLED;
}
irqreturn_t irq_right_button2(int irq, void *dev_id)
{
    //lidbg("irq_right_button2: %d\n", irq);
    return IRQ_HANDLED;
}
void button_init(void)
{
    lidbg("button_init\n");
    FS_REGISTER_INT(button_en, "button_en", 0, NULL);
    if(button_en)
    {
        INIT_WORK(&work_left_button1, work_left_button1_fn);
        INIT_WORK(&work_right_button1, work_right_button1_fn);

        SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, GPIO_CFG_PULL_UP);
        SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, GPIO_CFG_PULL_UP);
        SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, GPIO_CFG_PULL_UP);
        SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, GPIO_CFG_PULL_UP);

        SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_left_button1, NULL);
        SOC_IO_ISR_Add(BUTTON_LEFT_2, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_left_button2, NULL);
        SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_right_button1, NULL);
        SOC_IO_ISR_Add(BUTTON_RIGHT_2, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_right_button2, NULL);
    }
}
int thread_button_init(void *data)
{

    button_init();
    return 0;
}
