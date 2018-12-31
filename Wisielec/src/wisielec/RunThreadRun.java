/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package wisielec;

import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import static wisielec.Wisielec.clientSocket;
import static wisielec.Wisielec.reader;

/**
 *
 * @author Łoś
 */
public class RunThreadRun  implements Runnable {

    FXMLController a;
    String inputLine=null;
    public RunThreadRun(FXMLController A)
    {
        a=A;
    }
    @Override
    public void run(){
        while(true)
        {
         if(clientSocket!=null)
         {
            try {
                inputLine = reader.readLine();
            } catch (IOException ex) {
                Logger.getLogger(RunThreadRun.class.getName()).log(Level.SEVERE, null, ex);
            }
         System.out.println("Wiadomość: " + inputLine);
         
         }
         else
         try {
             TimeUnit.SECONDS.sleep(1);
         } catch (InterruptedException ex) {
             Logger.getLogger(RunThreadRun.class.getName()).log(Level.SEVERE, null, ex);
         }        
             Platform.runLater(() -> a.Hello(inputLine));

         
        }
       }
}
    

