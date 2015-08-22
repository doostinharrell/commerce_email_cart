<?php

/**
 * Implements hook_mail().
 */
function commerce_email_cart_mail($key, &$message, $params) {
  $site_name = variable_get('site_name');
  global $user;

  $options = array(
    'langcode' => $message['language']->language,
  );

  switch ($key) {
    // Send a simple message from the contact form.
    case 'contact_message':
      $message['subject'] = t('E-mail sent from @site-name', array('@site-name' => variable_get('site_name', 'Drupal')), $options);
      // Note that the message body is an array, not a string.
      $message['body'][] = t('Someone sent you a shopping cart for @site_name', array('@site_name' => $site_name), $options);
      $message['body'][] = check_plain($params['message']);
      $message['body'][] = commerce_cart_email_build_link();
      break;
  }
}

/**
 * Sends an e-mail.
 *
 * @param array $form_values
 *   An array of values from the contact form fields that were submitted.
 *   There are just two relevant items: $form_values['email'] and
 *   $form_values['message'].
 */
function commerce_email_cart_mail_send($form_values) {
  $module = 'commerce_email_cart';
  $key = 'contact_message';

  // Specify 'to' and 'from' addresses.
  $to = $form_values['email'];
  $from = variable_get('site_mail', 'admin@example.com');

  // "params" loads in additional context for email content.
  $params = $form_values;

  // The language of the e-mail.
  $language = language_default();

  // Enable email sending.
  $send = TRUE;

  // Display message indicating if the email was sent successfully.
  $result = drupal_mail($module, $key, $to, $language, $params, $from, $send);
  if ($result['result'] == TRUE) {
    drupal_set_message(t('Your message has been sent.'));
  }
  else {
    drupal_set_message(t('There was a problem sending your message and it was not sent.'), 'error');
  }

}

/**
 * Builds our cart email form.
 */
function commerce_email_cart_form() {
  // Enable form elements by default.
  $disabled = FALSE;

  // Ger user id.
  global $user;

  $uid = 0;
  if ($user->uid != 0) {
    $uid = $user->uid;
  }

  // Load current user's order id.
  $order_id = commerce_cart_order_load($uid);

  // If cart is empty disable form elements.
  if (!isset($order_id->commerce_line_items['und'])) {
    drupal_set_message(t('You need to add some products to your cart before you can send an email.'), 'error');
    $disabled = TRUE;
  }

  $form['intro'] = array(
    '#markup' => t('Use this form to send your current cart to an e-mail address.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
    '#disabled' => $disabled,
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#required' => TRUE,
    '#disabled' => $disabled,
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#required' => TRUE,
    '#default_value' => t('Here is a link to build your shopping cart'),
    '#disabled' => $disabled,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Email Cart'),
    '#disabled' => $disabled,
  );

  return $form;
}

/**
 * Form validation logic for the contact form.
 */
function commerce_email_cart_form_validate($form, &$form_state) {
  if (!valid_email_address($form_state['values']['email'])) {
    form_set_error('email', t('That e-mail address is not valid.'));
  }
}

/**
 * Form submission logic for the contact form.
 */
function commerce_email_cart_form_submit($form, &$form_state) {
  commerce_email_cart_mail_send($form_state['values']);
}
