
import gnu.io.CommPortIdentifier;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;

import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.Enumeration;
import java.util.TooManyListenersException;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class Main {
	static final String PORTNAME = "COM17";
	static final String FILENAME = "./src/data.txt";
	static SerialPort port = null;
	static InputStream input = null;
	static PrintWriter pw = null;
	static JLabel label = null;
	static JLabel xRawLabel = null;
	static JLabel yRawLabel = null;
	static JLabel xForseLabel = null;
	static JLabel yForseLabel = null;
	static final int X_COEFF = 1000;
	static final int Y_COEFF = 1000;
	static final int X_SHIFT = 0;
	static final int Y_SHIFT = 0;
	static final double FILTRATION = 0.1;
	static int xValue = 0;
	static int yValue = 0;
	static double xForse = 0;
	static double yForse = 0;
	
	public static double round(double d){
		int a = 1000;
		int b = (int) (a*d);
		return (double) b/a;
	}

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Enumeration<?> identifiers = CommPortIdentifier.getPortIdentifiers();
		CommPortIdentifier ident = null;
		while(identifiers.hasMoreElements()){
			ident = (CommPortIdentifier) identifiers.nextElement();
			if(ident.getPortType() == CommPortIdentifier.PORT_SERIAL){
				System.out.println(ident.getName());
				if(ident.getName().equals(PORTNAME)){
					try {
						port = (SerialPort) ident.open(Main.class.getName(), 1000);
						port.setSerialPortParams(9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_2, SerialPort.PARITY_NONE);
						port.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
						port.notifyOnDataAvailable(true);
						port.addEventListener(new SerialPortEventListener() {
							byte[] val1 = new byte[32];
							byte[] val2 = new byte[32];
							int len1 = 0;
							int len2 = 0;
							boolean b = false;
							boolean start = false;
							
							@Override
							public void serialEvent(SerialPortEvent arg0) {
								// TODO Auto-generated method stub
								switch(arg0.getEventType()){
								case SerialPortEvent.DATA_AVAILABLE:
									byte[] buf = new byte[32];
									int num = 0;
									try {
										while(input.available() > 0){
											num = input.read(buf);
											for(int i = 0; i<num; i++){
												if(start){
													switch(buf[i]){
													case '\t':
														b = true;
														break;
													case '\r':
														xValue = Integer.parseInt(new String(val1, 0, len1));
														yValue = Integer.parseInt(new String(val2, 0, len2));
														//pw.println(xValue + "\t" + yValue);
														xRawLabel.setText("raw value x: " + new String(val1, 0, len1));
														yRawLabel.setText("raw value y: " + new String(val2, 0, len2));
														xForse = (1 - FILTRATION)*(xValue - X_SHIFT)/X_COEFF + FILTRATION*xForse;
														yForse = (1 - FILTRATION)*(yValue - Y_SHIFT)/Y_COEFF + FILTRATION*yForse;
														xForseLabel.setText("Fx: " + new Double(round(xForse)).toString());
														yForseLabel.setText("Fy: " + new Double(round(yForse)).toString());
														label.repaint();
														len1 = 0;
														len2 = 0;
														b = false;
														break;
													case '\n':
														break;
													default:
														if(b) val2[len2++] = buf[i];
														else val1[len1++] = buf[i];
														break;
													}
												} else {
													if(buf[i] == '\n') start = true;
												}
											}
										}
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									break;
								}
							}
						});
						input = port.getInputStream();
					} catch (PortInUseException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (UnsupportedCommOperationException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (TooManyListenersException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					break;
				}
			}
		}
		if(port != null){
			JFrame wnd = new JFrame("forse visualizer");
			wnd.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			wnd.addWindowListener(new WindowListener() {
				
				@Override
				public void windowOpened(WindowEvent e) {}
				
				@Override
				public void windowIconified(WindowEvent e) {}
				
				@Override
				public void windowDeiconified(WindowEvent e) {}
				
				@Override
				public void windowDeactivated(WindowEvent e) {}
				
				@Override
				public void windowClosing(WindowEvent e) {
					// TODO Auto-generated method stub
					port.removeEventListener();
					port.close();
					pw.close();
				}
				
				@Override
				public void windowClosed(WindowEvent e) {}
				
				@Override
				public void windowActivated(WindowEvent e) {}
			});
			wnd.setSize(800, 800);
			ImageIcon icon = new ImageIcon("./src/profile.png");
			label = new JLabel(icon, JLabel.CENTER){
				private static final long serialVersionUID = 1L;
				
				final int X_CENTER = 330;
				final int Y_CENTER = 355;
				final int SCALE = 500;

				@Override
				protected void paintComponent(Graphics arg0) {
					// TODO Auto-generated method stub
					super.paintComponent(arg0);
					((Graphics2D) arg0).setStroke(new BasicStroke(5));
					arg0.setColor(Color.BLUE);
					int x = (int) (SCALE*xForse);
					int y = (int) (SCALE*yForse);
					arg0.drawLine(X_CENTER, Y_CENTER, X_CENTER + x, Y_CENTER - y);
				}
			};
			wnd.getContentPane().setBackground(Color.WHITE);
			wnd.add(label, BorderLayout.CENTER);
			xRawLabel = new JLabel();
			yRawLabel = new JLabel();
			xForseLabel = new JLabel();
			yForseLabel = new JLabel();
			JPanel nPanel = new JPanel();
			JPanel sPanel = new JPanel();
			nPanel.setLayout(new GridLayout());
			sPanel.setLayout(new GridLayout());
			nPanel.add(xRawLabel);
			nPanel.add(yRawLabel);
			sPanel.add(xForseLabel);
			sPanel.add(yForseLabel);
			wnd.add(nPanel, BorderLayout.NORTH);
			wnd.add(sPanel, BorderLayout.SOUTH);
			wnd.setVisible(true);
			try {
				pw = new PrintWriter(new File(FILENAME));
			} catch (FileNotFoundException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
	}
}
