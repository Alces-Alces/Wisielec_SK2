/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package wisielec;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import static wisielec.Wisielec.clientSocket;
import static wisielec.Wisielec.reader;

/**
 * FXML Controller class
 *
 * @author wisni
 */
public class FXMLController implements Initializable {

    @FXML
    private TextField TextLabel;
    @FXML
    private TextField TextWynik;
    @FXML
    private TextField IP_place;
    @FXML
    private TextField Domain_place;

    @FXML
    private Button Button;
    @FXML
    private Button WyslijButton;

    @FXML
    private TextArea TextArea;

    /**
     * Initializes the controller class.
     */
    @FXML
    private void MainButton(ActionEvent event) throws IOException {
        //String serverMessage = "";
        //InputStream is = clientSocket.getInputStream();
        //byte[] buffer = new byte[100];
        //is.read(buffer);

        //serverMessage = reader.readLine();
        String inputLine;
        inputLine = reader.readLine();
        //while ((inputLine = reader.readLine()) != null) {
        System.out.println("Wiadomość: " + inputLine);
        //}

        //while (reader != null) {
        //    String serverMessage = reader.readLine();
        //    System.out.println("Wiadomość: " + serverMessage);
        //}
        //BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        //StringBuilder sb = new StringBuilder();
        //String line;
        //while ((line = reader.readLine()) != null) {
        //    sb.append(line).append("\n");
        //}
    }

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        try {
            //TO DO
            RunThreadRun runner = new RunThreadRun(this);
            Thread thread = new Thread(runner);
            thread.start();
        } catch (IOException ex) {
            Logger.getLogger(FXMLController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public void Hello(String a) {
        StringBuilder message = new StringBuilder(a);
        System.out.println("Metoda Hello: "+message);
        while (message.indexOf("-") != -1) {
            int isFound = message.indexOf("-");     
            message.setCharAt(isFound, '\n');
            System.out.println("Metoda Hello: "+message);
        }

        TextWynik.setText(a);
        TextArea.appendText(message + "\n");
    }

    @FXML
    private void WyslijAction(ActionEvent event) throws IOException {
        String clientMessage = TextLabel.getText();
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println(clientMessage);
        TextLabel.clear();
    }

    @FXML
    private void PolaczAction(ActionEvent event) throws IOException {
        if (clientSocket != null) {
            clientSocket.close();
        }
        String Ip_string = IP_place.getText();
        clientSocket = new Socket(Ip_string, 8888); //Połącz z serwerem
    }

    @FXML
    private void DomainAction(ActionEvent event) throws IOException {
        if (clientSocket != null) {
            clientSocket.close();
        }
        String domain = Domain_place.getText();
        clientSocket = new Socket(java.net.InetAddress.getByName(domain), 8080); //Połącz z serwerem
    }

}
