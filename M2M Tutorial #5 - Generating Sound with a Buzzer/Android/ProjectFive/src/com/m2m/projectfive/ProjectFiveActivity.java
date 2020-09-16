package com.m2m.projectfive;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;
//Use this for Android 3.1+ instead of com.android.future.usb.* 
//import android.hardware.usb.UsbAccessory;
//import android.hardware.usb.UsbManager;

public class ProjectFiveActivity extends Activity {
	
	// The constant TAG is an identifier for the current class and is used only for logging purposes with logcat.
	private static final String TAG = ProjectFiveActivity.class.getSimpleName();
	
	// Establishing a connection to an external device has to be permitted by the user.
	// When the user is granting the rights to connect your ADK board, the PendingIntent will broadcast the ACTION_USB_PERMISSION with a flag reflecting wheter the user confirmed or denierd the access.
	// The boolean variable mPermissionRequestPending is only used to not show the permission dialog again if the user interaction is still pending.
	private PendingIntent mPermissionIntent;
	private static final String ACTION_USB_PERMISSION = "com.m2m.projectfive.action.USB_PERMISSION";
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
	
	// Same constants as the ones used in the Arduino sketch.
	private static final byte COMMAND_ANALOG = 0x3;
	private static final byte TARGET_PIN_2 = 0x2;
	
	// The only user-visible UI elements is a Spinner which allow user to select a note 
	private Spinner notesSpinner;
	private ArrayAdapter<CharSequence> adapter;
	
	private int[] notes = { /*C3*/ 131, /*D3 */ 147, /*E3 */ 165, /*F3 */ 175, /*G3 */ 196, /*A3 */ 220, /* B3 */ 247 };
	
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
		
		// The intent filter is used in conjunction with a broadcast receiver to make sure that the application only listens to certain broadcasts
		IntentFilter filter = new IntentFilter (ACTION_USB_PERMISSION);

		// The filter defines that it reacts on the ACTION_USB_PERMISSION action defined as a constant in the beginning and on the ACTION_USB_ACCESSORY_DETACHED action, for when the ADK accessory is disconnected.
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		
		// The registerReceiver method register the broadcast receiver with the described intent filter at the system.
		// So when a broadcast is sent by the system, the broadcast receiver will be notified and can take the relevant action.
		registerReceiver(mUsbReceiver, filter);
		
		// Display main content including a Spinner and its listener
		setContentView(R.layout.main);
		notesSpinner = (Spinner) findViewById(R.id.spinner);
		notesSpinner.setOnItemSelectedListener(onItemSelectedListener);
		adapter = ArrayAdapter.createFromResource(this, R.array.notes, android.R.layout.simple_spinner_item);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		notesSpinner.setAdapter(adapter);
	}

	
	// Called when the activity is resumed from its paused state and immediately after onCreate()
	// It's here you open the connection to your accessory
	@Override
	public void onResume() {
		super.onResume();

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
	}
	
	// Called when the activity is no longer need to prior to being removed from the activity stack.
	// It's here you unregister the BroadcastReceiver
	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(mUsbReceiver);
	}
	
	// Listener responsible for handling the case in which a value has been selected
	OnItemSelectedListener onItemSelectedListener = new OnItemSelectedListener() {

		@Override
		public void onItemSelected(AdapterView<?> adapterView, View view, int position, long id) {

			new AsyncTask<Integer, Void, Void>() {

				@Override
				protected Void doInBackground(Integer... params) {
					// Send the note to ADK Board
					sendAnalogValueCommand(TARGET_PIN_2, notes[params[0]]);
					return null;
				}
			}.execute(position);
		}

		@Override
		public void onNothingSelected(AdapterView<?> arg0) {
			// not implemented
		}
	};

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
	
	// Send the note to ADK Board
	public void sendAnalogValueCommand(byte target, int value) {
		byte[] buffer = new byte[6];
		buffer[0] = COMMAND_ANALOG;
		buffer[1] = target;
		buffer[2] = (byte) (value >> 24);
		buffer[3] = (byte) (value >> 16);
		buffer[4] = (byte) (value >> 8);
		buffer[5] = (byte) value;
		
		if (mOutputStream != null) {
			try {
				mOutputStream.write(buffer);
			} catch (IOException e) {
				Log.e(TAG, "write failed", e);
			}
		}
	}
}