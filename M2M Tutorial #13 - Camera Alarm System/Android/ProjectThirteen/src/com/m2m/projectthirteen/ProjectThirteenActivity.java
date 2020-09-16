package com.m2m.projectthirteen;

import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import android.app.Activity;
import android.app.PendingIntent;
import android.app.PendingIntent.CanceledException;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.PictureCallback;
import android.os.Bundle;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;
import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;
//Use this for Android 3.1+ instead of com.android.future.usb.* 
//import android.hardware.usb.UsbAccessory;
//import android.hardware.usb.UsbManager;

public class ProjectThirteenActivity extends Activity {
	
	// The constant TAG is an identifier for the current class and is used only for logging purposes with logcat.
	private static final String TAG = ProjectThirteenActivity.class.getSimpleName();

	// Establishing a connection to an external device has to be permitted by the user.
	// When the user is granting the rights to connect your ADK board, the PendingIntent will broadcast the ACTION_USB_PERMISSION with a flag reflecting wheter the user confirmed or denierd the access.
	// The boolean variable mPermissionRequestPending is only used to not show the permission dialog again if the user interaction is still pending.
	private PendingIntent mPermissionIntent;
	private static final String ACTION_USB_PERMISSION = "com.m2m.projectthirteen.action.USB_PERMISSION";
	private boolean mPermissionRequestPending;

	// The UsbManager is a system service that manages all interaction with the USB port of the device.
	// It is used to enumerate the connected devices and to request and check the permission to connect to an accessory.
	// The UsbManager is also responsible for opening the connection to the external device.
	private UsbManager mUsbManager;
	
	// The UsbAccessory is a reference to the connected accessory.
	private UsbAccessory mAccessory;
	
	// The ParcelFileDescriptor is obtained when the connection to the accessory is established
	// It is used to get access to the input- and output stream of the accessory.
	private ParcelFileDescriptor mFileDescriptor;
	private FileInputStream mInputStream;
	private FileOutputStream mOutputStream;

	// SMS Intent & Log File Intent
	private PendingIntent photoTakenIntent;
	private PendingIntent logFileWrittenIntent;
	
	// Same constants as the ones used in the Arduino sketch.
	private static final byte COMMAND_ALARM = 0x9;
	private static final byte ALARM_TYPE_IR_LIGHT_BARRIER = 0x2;
	private static final byte ALARM_OFF = 0x0;
	private static final byte ALARM_ON = 0x1;
	
	private static final String PHOTO_TAKEN_ACTION = "PHOTO_TAKEN";
	private static final String LOG_FILE_WRITTEN_ACTION = "LOG_FILE_WRITTEN";
	
	// Create PackageManager instance to find global package information. 
	private PackageManager packageManager;
	
	// Boolean to test if smartphone has 1 or 2 camera
	private boolean hasFrontCamera;
	private boolean hasBackCamera;

	// Camera params
	private Camera camera;
	private SurfaceView surfaceView;
	
	// The only user-visible UI elements are TextView, LinearLayout & FrameLayout
	private TextView alarmTextView;
	private TextView photoTakenTextView;
	private TextView logTextView;
	private LinearLayout linearLayout;
	private FrameLayout frameLayout;
	
	/** Called when the activity is first created */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// A reference to the USB system service is obtained so that you can call its methods later on
		mUsbManager = UsbManager.getInstance(this);
		
		// Use this for Android 3.1+
		// mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
		
		// A PendingIntent with the ACTION_USB_PERMISSION parameter is defined
		// You will need it when you request the user's permission to connect to a USB device
		mPermissionIntent = PendingIntent.getBroadcast(this,0, new Intent(ACTION_USB_PERMISSION),0);
		
		// A PendingIntent with the PHOTO_TAKEN_ACTION parameter is defined
		photoTakenIntent = PendingIntent.getBroadcast(this, 0, new Intent(PHOTO_TAKEN_ACTION), 0);
		
		// A PendingIntent with the LOG_FILE_WRITTEN_ACTION parameter is defined
		logFileWrittenIntent = PendingIntent.getBroadcast(this, 0, new Intent(LOG_FILE_WRITTEN_ACTION), 0);
		
		// The intent filter is used in conjunction with a broadcast receiver to make sure that the application only listens to certain broadcasts
		IntentFilter filter = new IntentFilter (ACTION_USB_PERMISSION);
		
		// The filter defines that it reacts on the ACTION_USB_PERMISSION action defined as a constant in the beginning and on the ACTION_USB_ACCESSORY_DETACHED action, for when the ADK accessory is disconnected.
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		
		// The filter defines that it reacts on the PHOTO_TAKEN_ACTION action defined
		filter.addAction(PHOTO_TAKEN_ACTION);
		
		// The filter defines that it reacts on the LOG_FILE_WRITTEN_ACTION action defined
		filter.addAction(LOG_FILE_WRITTEN_ACTION);
		
		// The registerReceiver method register the broadcast receiver with the described intent filter at the system.
		// So when a broadcast is sent by the system, the broadcast receiver will be notified and can take the relevant action.
		registerReceiver(mUsbReceiver, filter);
		
		// Find global package information
		packageManager = getPackageManager();
		
		// To count how much camera the smartphone has
		hasFrontCamera = packageManager.hasSystemFeature(PackageManager.FEATURE_CAMERA_FRONT);
		hasBackCamera = packageManager.hasSystemFeature(PackageManager.FEATURE_CAMERA);
		
		// Display main content
		setContentView(R.layout.main);

		// UI params
		linearLayout = (LinearLayout) findViewById(R.id.linear_layout);
		frameLayout = (FrameLayout) findViewById(R.id.camera_preview);
		alarmTextView = (TextView) findViewById(R.id.alarm_text);
		photoTakenTextView = (TextView) findViewById(R.id.photo_taken_text);
		logTextView = (TextView) findViewById(R.id.log_text);
	}
	
	// Called when the activity is resumed from its paused state and immediately after onCreate()
	// It's here you open the connection to your accessory
	@Override
	public void onResume() {
		super.onResume();
		
		// Classic Camera initialization
		camera = getCamera();
		surfaceView = new CameraPreview(this,camera);
		frameLayout.addView(surfaceView);
		
		// If the input and outputStream is still active you are good to go for communication and can return prematurely
		if(mInputStream != null && mOutputStream != null) {
			return;
		}
		
		// Get reference of the accessory from the UsbManager
		UsbAccessory [] accessories = mUsbManager.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		
		if (accessory !=null) {
		
			// If you already have the user's permission to communicate with the device you can open and reassign the input and output streams
			if (mUsbManager.hasPermission(accessory)) {
				openAccessory(accessory);
			} else {
				synchronized (mUsbReceiver) {
					if(!mPermissionRequestPending) {
						mUsbManager.requestPermission(accessory, mPermissionIntent);
						mPermissionRequestPending = true;
					}
				}
			}
		} else {
			Log.d(TAG, "mAccessory is null");
		}
	}
	
	// Called when the activity is paused by the system.
	// It's here you close the connection to free up memory.
	@Override
	public void onPause() {
		super.onPause();
		closeAccessory();
		
		if(camera != null) {
			camera.release();
			camera = null;
			frameLayout.removeAllViews();
		}
	}
	
	// Called when the activity is no longer need to prior to being removed from the activity stack.
	// It's here you unregister the BroadcastReceiver
	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(mUsbReceiver);
	}
	
	// Implementation of the BroadcastReceiver.
    // Anonymous inner class
	private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {

		// Override the onReceive method which is called by the system if this broadcast receiver is registered
		// and matches the provided intent-filter.
		@Override
		public void onReceive(Context context, Intent intent) {
			
			// Check which action occurred when the broadcast receiver is called.
			String action = intent.getAction();
			
			// If you receive the action describing that a permission request has been answered
			// you will have to check if the user granted permission to communicate with your accessory.
			if (ACTION_USB_PERMISSION.equals(action)) {
				synchronized (this) {
					
					// Used for Android 2.3
					UsbAccessory accessory = UsbManager.getAccessory(intent);
					
					// Use this for Android 3.1+
					// UsbAccessory accessory = (UsbAccessory) intent.getParcelableExtra(UsbManager.EXTRA_PERMISSION_GRANTED);

					if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
						
						// Permission accepted ! We open the communication channels of the accessory !
						openAccessory(accessory);
						
					} else {
						Log.d(TAG, "permission denied for accessory" + accessory);
					}
					mPermissionRequestPending = false;
				}

			// The second action which could have triggered the broadcast receiver
			// is the notification that the accessory has been detached from Android device.
			} else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {

				// Use this for Android 2.3.3
				UsbAccessory accessory = UsbManager.getAccessory(intent);
				
				// Use this for Android 3.1+
				//UsbAccessory accessory = (UsbAccessory) intent.getParcelableExtra(UsbManager.EXTRA_PERMISSION_GRANTED);
				
				if (accessory != null && accessory.equals(mAccessory)) {
					closeAccessory();
				}
			} else if (PHOTO_TAKEN_ACTION.equals(action)) {
				photoTakenTextView.setText(R.string.photo_taken_message);
			} else if (LOG_FILE_WRITTEN_ACTION.equals(action)) {
				logTextView.setText(R.string.log_written_message);
			}
		}
	};
		
	// Open the communication channels to the accessory
	private void openAccessory(UsbAccessory accessory) {
		
		// Delegate to the USB service method to obtain a FileDescriptor for your accessory.		
		mFileDescriptor = mUsbManager.openAccessory(accessory);
		
		if (mFileDescriptor != null) {
			mAccessory = accessory;
			
			// The FileDescriptor manages the input- output stream used to communicate with your device
			FileDescriptor fd = mFileDescriptor.getFileDescriptor();
			mInputStream = new FileInputStream(fd);
			mOutputStream = new FileOutputStream(fd);
			Thread thread = new Thread(null, commRunnable, TAG);
			thread.start();
			Log.d(TAG, "accessory opened");
		} else {
			Log.d(TAG, "accessory open fail");
		}
	}

	// Close the communication channels to the accessory
	private void closeAccessory() {
		try {
			if (mFileDescriptor != null) {
				mFileDescriptor.close();
			}
		} catch (IOException e) {
		} finally {
			mFileDescriptor = null;
			mAccessory = null;
		}
	}

	// Thread to manage incoming data from Arduino
	Runnable commRunnable = new Runnable() {
		
		@Override
		public void run() {
			int ret = 0;
			byte[] buffer = new byte[3];
			
			while(ret >= 0) {
				try {
					ret = mInputStream.read(buffer);
				} catch (IOException e) {
					Log.e(TAG, "IOException : ", e);
					break;
				}
				
				switch (buffer[0]) {
					case COMMAND_ALARM :
						if(buffer[1] == ALARM_TYPE_IR_LIGHT_BARRIER) {
							final byte alarmState = buffer[2];
							final String alarmMessage = getString(R.string.alarm_message, getString(R.string.alarm_type_ir_light_barrier));
							
							runOnUiThread (new Runnable() {
								@Override
								public void run() {
									if (alarmState == ALARM_ON) {
										linearLayout.setBackgroundColor(Color.RED);
										alarmTextView.setText(alarmMessage);
									} else if (alarmState == ALARM_OFF) {
										linearLayout.setBackgroundColor(Color.WHITE);
										alarmTextView.setText(R.string.alarm_reset_message);
										photoTakenTextView.setText("");
										logTextView.setText("");
									}
								}
							});
							if (alarmState == ALARM_ON) {
								takePhoto();
								writeToLogFile(new StringBuilder(alarmMessage).append(" - ").append("date").toString());
							} else if (alarmState == ALARM_OFF) {
								camera.startPreview();
							}
						}
						break;
						
					default:
						Log.d(TAG, "unknown msg : " + buffer[0]);
						break;
				}
			}
		}
	};
	
	// Choose available Camera
	private Camera getCamera() {
		Camera camera = null;
		
		try {
			if (hasFrontCamera) {
				int frontCameraId = getFrontCameraId();
				if (frontCameraId != -1) {
					camera = Camera.open(frontCameraId);
				}
			}
			if ((camera == null) && hasBackCamera) {
				camera = Camera.open();
			}
		} catch (Exception e) {
			Log.d(TAG, "Camera could not be initialized", e);
		}
		return camera;
	}
	
	// Get Front Camera Id
	private int getFrontCameraId() {
		int cameraId = -1;
		int numberOfCameras = Camera.getNumberOfCameras();
		for (int i = 0; i< numberOfCameras; i++) {
			CameraInfo cameraInfo = new CameraInfo();
			Camera.getCameraInfo(i, cameraInfo);
			if(CameraInfo.CAMERA_FACING_FRONT == cameraInfo.facing) {
				cameraId = i;
				break;
			}
		}
		return cameraId;
	}
	
	// Take Photo method
	private void takePhoto() {
		if(camera != null) {
			camera.takePicture(null, null, pictureTakenHandler);
		}
	}
	
	// Picture Callback Handler
	private PictureCallback pictureTakenHandler = new PictureCallback() {

		@Override
		public void onPictureTaken(byte[] data, Camera camera) {
			writePictureDataToFile(data);
		}
	};
	
	// Write to log file method
	private void writeToLogFile(String logMessage) {
		File logFile = getFile("ProjectThirteenLog.txt");
		
		if (logFile != null) {

			BufferedWriter bufferedWriter = null;
			
			try {
				bufferedWriter = new BufferedWriter (new FileWriter(logFile, true));
				bufferedWriter.write(logMessage);
				bufferedWriter.newLine();
				Log.d(TAG, "Written message to file : " + logFile.toURI());
				logFileWrittenIntent.send();
			} catch (IOException e) {
				Log.d(TAG, "Could not write to Log File.", e);
			} catch (CanceledException e) {
				Log.d(TAG, "LogFileWrittenIntent was cancelled.", e);
			} finally {
				if (bufferedWriter != null) {
					try {
						bufferedWriter.close();
					} catch (IOException e) {
						Log.d(TAG, "Could not close Log File.",e);
					}
				}
			}
		}
	}
	
	// Write Picture to file method
	private void writePictureDataToFile(byte[] data) {
		SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
		String currentDateAndTime = dateFormat.format(new Date());
		File pictureFile = getFile(currentDateAndTime + ".jpg");
		if(pictureFile != null) {
			BufferedOutputStream bufferedOutputStream = null;
			try {
				bufferedOutputStream = new BufferedOutputStream (new FileOutputStream(pictureFile));
				bufferedOutputStream.write(data);
				Log.d(TAG, "Written picture data to file : " + pictureFile.toURI());
				photoTakenIntent.send();
			} catch (IOException e) {
				Log.d(TAG,"Could not write to Picture File.", e);
			} catch (CanceledException e) {
				Log.d(TAG, "photoTakenIntent was cancelled.", e);
			} finally {
				if(bufferedOutputStream != null) {
					try {
						bufferedOutputStream.close();
					} catch (IOException e) {
						Log.d(TAG, "Could not close Picture File.",e);
					}
				}
			}
		}
	}
	
	// File management method
	private File getFile(String fileName) {
		File file = new File(getExternalDir(), fileName);
		if (!file.exists()) {
			try {
				file.createNewFile();
			} catch (IOException e) {
				Log.d(TAG, "File could not be created.",e);
			}
		}
		return file;
	}
	
	// Storage device management method
	private File getExternalDir() {
		
		// Returns the current state of the primary "external" storage device.
		String state = Environment.getExternalStorageState();
		
		//Storage state if the media is present and mounted at its mount point with read/write access.
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			return getExternalFilesDir(null);
		} else {
			return null;
		}
	}
}