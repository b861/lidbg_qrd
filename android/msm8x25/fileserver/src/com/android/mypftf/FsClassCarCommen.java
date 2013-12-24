package com.android.mypftf;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.FileOutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.telephony.gsm.SmsManager;
import android.widget.Toast;

import org.apache.http.util.EncodingUtils;

/**
 * @author futengfeimyp
 * 
 */
public class FsClassCarCommen
{
	private static final String NULL = null;
	Context gcontext;
	private WifiManager mWifiManager;
	private Runtime tmpRuntime;

	/**
	 * 
	 */
	public FsClassCarCommen(Context context)
	{
		// TODO Auto-generated constructor stub
		gcontext = context;
		tmpRuntime = Runtime.getRuntime();
		mWifiManager = (WifiManager) gcontext
				.getSystemService(Context.WIFI_SERVICE);
	}

	public void intent_open_file(String path, String action)
	{
		// TODO Auto-generated method stub
		File file = new File(path);
		Intent intent = new Intent();
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.setAction(action);
		String type = "text/plain";
		intent.setDataAndType(Uri.fromFile(file), type);
		gcontext.startActivity(intent);
	}

	public String get_file_content(String pathString)
	{

		String fineString = "null";
		File tmpfFile = new File(pathString);
		if (!tmpfFile.exists())
		{
			return fineString;
		}
		// TODO Auto-generated method stub
		try
		{
			FileInputStream inputStream = new FileInputStream(tmpfFile);
			int len = inputStream.available();
			if (len == 0)
			{
				len = 500;
			}
			byte[] buffer = new byte[len];
			inputStream.read(buffer);
			fineString = EncodingUtils.getString(buffer, "UTF-8");
			// toast_show("resString="+fineString);
			inputStream.close();

		} catch (FileNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return fineString;
	}

	public String get_machine_id(String pathString)
	{
		// TODO Auto-generated method stub
		return get_file_content(pathString);
	}

	public void write_to_file(String file_path, boolean creatit,
			String write_str)
	{
		// TODO Auto-generated method stub

		if (write_str == null)
		{
			return;
		}
		File mFile = new File(file_path);
		if (!mFile.exists())
		{
			if (creatit)
			{
				try
				{
					mFile.createNewFile();
				} catch (IOException e)
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else
			{
				show_toast(file_path + "：不存在");
				return;
			}
		}

		try
		{
			FileOutputStream fout = new FileOutputStream(
					mFile.getAbsolutePath());
			byte[] bytes = write_str.getBytes();
			fout.write(bytes);
			fout.close();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (creatit)
		{
			show_toast("已存入:" + file_path);
		}
	}

	public void show_toast(String toast_string)
	{
		// TODO Auto-generated method stub
		if (toast_string != null)
		{
			Toast.makeText(gcontext, toast_string, Toast.LENGTH_SHORT).show();
		}
	}

	public void show_toast_longer(String toast_string)
	{
		// TODO Auto-generated method stub
		Toast.makeText(gcontext, toast_string, Toast.LENGTH_LONG).show();
	}

	public String get_string_context_line(String string, String context)
	{
		// TODO Auto-generated method stub
		String[] tmp = string.split("\n");
		for (int i = 0; i < tmp.length; i++)
		{
			if (tmp[i].contains(context))
			{
				return tmp[i];
			}
		}
		return "null";
	}

	public String get_http_file(String urlStr)
	{
		// TODO Auto-generated method stub
		StringBuffer sb = new StringBuffer();
		String line = null;
		BufferedReader buffer = null;
		try
		{
			buffer = new BufferedReader(new InputStreamReader(
					getInputStreamFromUrl(urlStr)));
			while ((line = buffer.readLine()) != null)
			{
				sb.append(line);
			}
		} catch (Exception e)
		{
			e.printStackTrace();
		} finally
		{
			try
			{
				buffer.close();
			} catch (IOException e)
			{
				e.printStackTrace();
			}
		}
		show_toast(sb.toString());
		return sb.toString();
	}

	private InputStream getInputStreamFromUrl(String urlStr)
	{
		InputStream inputStream = null;
		try
		{
			URL url = new URL(urlStr);
			HttpURLConnection urlConn = (HttpURLConnection) url
					.openConnection();
			inputStream = urlConn.getInputStream();
		} catch (MalformedURLException e)
		{
			e.printStackTrace();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		return inputStream;
	}

	public ArrayList<File> get_dirfile_list(String filePath)
	{
		// TODO Auto-generated method stub
		ArrayList<File> txtlist = new ArrayList<File>();
		File f = new File(filePath);
		if (f.exists())
		{
			File[] files = f.listFiles();// note:你可以使用new
			if (files != null)
			{
				for (int i = 0; i < files.length; i++)
				{
					int filelength = files[i].getName().length();
					if (files[i].getName().substring(filelength - 3)
							.equals("txt")
							&& !files[i].getName().equals("state.txt"))
					{
						txtlist.add(files[i]);
					}
				}
			}
		} else
		{
			show_toast(filePath + "：未成功搜索到TXT文件");
		}
		return txtlist;
	}

	public boolean is_file_exist(String file)
	{
		// TODO Auto-generated method stub
		File kmsgfiFile = new File(file);
		if (kmsgfiFile.exists())
		{
			return true;
		} else
		{
			show_toast("文件不存在：" + file);
			return false;
		}

	}

	public boolean iswifiactivy()
	{
		// TODO Auto-generated method stub
		if (mWifiManager.isWifiEnabled())
		{
			return true;
		}
		return false;

	}

	public String getHostIpAddress()
	{
		// TODO Auto-generated method stub
		WifiInfo wi = mWifiManager.getConnectionInfo();
		int ipAdd = wi.getIpAddress();
		return (ipAdd & 0xFF) + "." + ((ipAdd >> 8) & 0xFF) + "."
				+ ((ipAdd >> 16) & 0xFF) + "." + (ipAdd >> 24 & 0xFF);
	}

	public String runRootCommend(String RootCommend)
	{
		// TODO Auto-generated method stub
		StringBuffer stringBuffer = null;
		DataOutputStream os = null;
		Process rootCommendProcess = null;
		try
		{
			rootCommendProcess = tmpRuntime.exec("su");
			String newcmdString = RootCommend + "\n";
			os = new DataOutputStream(rootCommendProcess.getOutputStream());
			os.write(newcmdString.getBytes());
			os.write("exit\n".getBytes());
			os.flush();
			try
			{
				rootCommendProcess.waitFor();
			} catch (InterruptedException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			stringBuffer = new StringBuffer();
			DataInputStream dataInputStream = new DataInputStream(
					rootCommendProcess.getInputStream());
			String line;
			while ((line = dataInputStream.readLine()) != NULL)
			{
				stringBuffer.append(line).append("\n");
			}

		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();

		} finally
		{
			if (os != null)
			{
				try
				{
					os.close();
				} catch (IOException e)
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			rootCommendProcess.destroy();
		}
		if (stringBuffer != null)
		{
			return stringBuffer.toString();
		} else
		{
			return null;
		}
	}

	public String GetRawFileContent(int RawId)
	{
		// TODO Auto-generated method stub
		String fineString = null;
		InputStream inputStream = gcontext.getResources()
				.openRawResource(RawId);
		int len;
		try
		{
			len = inputStream.available();
			if (len == 0)
			{
				len = 500;
			}
			byte[] buffer = new byte[len];
			inputStream.read(buffer);
			fineString = EncodingUtils.getString(buffer, "UTF-8");
			// toast_show("resString="+fineString);
			inputStream.close();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return fineString;
	}

	public String GetCurrentTime(String format)
	{
		// TODO Auto-generated method stub
		SimpleDateFormat formatter = new SimpleDateFormat(format);
		Date curDate = new Date(System.currentTimeMillis());// 获取当前时间
		String str = formatter.format(curDate);
		return str;
	}

	public void send_msg(String num, String context)
	{
		// TODO Auto-generated method stub
		SmsManager.getDefault().sendTextMessage(num, null, context, null, null);
	}

}
