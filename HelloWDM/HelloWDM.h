
#ifdef __cplusplus
extern "C"
{
#endif
#include <wdm.h>
#ifdef __cplusplus
}
#endif // __cplusplus


typedef struct _DEVICE_EXTENSION
{
	PDEVICE_OBJECT fdo;
	PDEVICE_OBJECT NextStackDevice;
	UNICODE_STRING ustrDeviceName; // �豸��
	UNICODE_STRING ustrSymLinkName; // ����������
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define PAGEDCODE code_seg("PAGE")
#define LOCKDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKDDATA data_seg()
#define INITDATA data_seg("INIT")

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

NTSTATUS HelloWDMAddDevice(IN PDRIVER_OBJECT DriverObject, IN PDRIVER_OBJECT PhysicalDeviceObject);

NTSTATUS HelloWDMPnp(IN PDRIVER_OBJECT fdo, IN PIRP Irp);

NTSTATUS HelloWDMDispatchRoutine(IN PDRIVER_OBJECT fdo, IN PIRP Irp);

void HelloWDMUnload(IN PDRIVER_OBJECT DriverObject);

extern "C"
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
