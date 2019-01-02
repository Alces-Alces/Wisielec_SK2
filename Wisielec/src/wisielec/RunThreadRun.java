/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package wisielec;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import static wisielec.Wisielec.clientSocket;

/**
 *
 * @author Łoś
 */
public class RunThreadRun  implements Runnable {
    public static BufferedReader reader;
    FXMLController a;
    String inputLine=null;
    public RunThreadRun(FXMLController A) throws IOException
    {
        a=A;
        
    }
    @Override
    public void run(){
        while(true)
        {
         if(clientSocket!=null)
         {   
             /*
             try {
             reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
             } catch (IOException ex) {
             Logger.getLogger(RunThreadRun.class.getName()).log(Level.SEVERE, null, ex);
             }
             
             try {
             inputLine = reader.readLine();
             } catch (IOException ex) {
             Logger.getLogger(RunThreadRun.class.getName()).log(Level.SEVERE, null, ex);
             }
             System.out.println("Wiadomość: " + inputLine);
             */
             try {
             reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
             String inputLine;
             inputLine = reader.readLine();
             Platform.runLater(() -> a.Hello(inputLine));
             //while ((inputLine = reader.readLine()) != null) {
             System.out.println("Wiadomość: " + inputLine);
             } catch (IOException ex) {
                 Logger.getLogger(RunThreadRun.class.getName()).log(Level.SEVERE, null, ex);
             }
         }
         else
         try {
             TimeUnit.SECONDS.sleep(1);
         } catch (InterruptedException ex) {
             Logger.getLogger(RunThreadRun.class.getName()).log(Level.SEVERE, null, ex);
         }        
             //Platform.runLater(() -> a.Hello("XD"));

         
        }
       }
}
    

